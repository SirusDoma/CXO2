#include <CXO2/Services/MusicDownloaderService.hpp>

#include <CXO2/Contexts/CommandLineContext.hpp>
#include <CXO2/Contexts/SessionContext.hpp>

#include <SFML/Network/Dns.hpp>
#include <SFML/Network/Ftp.hpp>
#include <SFML/Network/Sftp.hpp>
#include <SFML/System/Clock.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <optional>

namespace Cx
{
    namespace
    {
        constexpr auto UserName        = "anonymous";
        constexpr auto Password        = "webmaster@o2-media.com";
        constexpr auto MusicDirectory  = "Music";
        constexpr auto TempDirectory   = "Music/TEMP";
        constexpr auto RemoteDirectory = "Patch/MUSIC/";

        constexpr unsigned short FtpPort  = 21;
        constexpr unsigned short SftpPort = 22;

        constexpr auto ConnectTimeout  = sf::seconds(5);
        constexpr auto TransferTimeout = sf::seconds(30);
        constexpr auto PollingPeriod   = sf::milliseconds(50);

        struct FileEntry
        {
            std::string Name;
            std::uint64_t Size;
            int Status;
        };

        std::string GetTempFileName(const std::uint16_t musicID, const std::uint16_t version)
        {
            auto name = "o2ma" + std::to_string(musicID);
            if (version != 0)
                name += "_" + std::to_string(version);

            return name;
        }

        void WriteFileInfo(const std::filesystem::path& path, const std::vector<FileEntry>& entries)
        {
            auto file = std::ofstream(path, std::ios::trunc);
            file << "[FileInfo]\n";
            file << "FileNum=" << entries.size() << "\n";
            for (std::size_t i = 0; i < entries.size(); i++)
            {
                file << "FileName" << (i + 1) << "=" << entries[i].Name << "\n";
                file << "FileStatus" << (i + 1) << "=" << entries[i].Status << "\n";
            }
        }
    }

    MusicDownloaderService::MusicDownloaderService(CommandLineContext& args, SessionContext& session) :
        m_args(args),
        m_session(session)
    {
    }

    MusicDownloaderService::~MusicDownloaderService()
    {
        m_cancelled = true;
        if (m_thread.joinable())
            m_thread.join();
    }

    std::size_t MusicDownloaderService::Enqueue(const std::uint16_t musicID, const std::uint16_t version)
    {
        const auto& musicList = m_session.GetMusicList();
        const auto metadata = std::find_if(musicList.begin(), musicList.end(), [musicID](const ChartMetadata& entry)
        {
            return entry.ID == musicID;
        });

        auto lock = std::lock_guard(m_mutex);
        m_queue.push_back({musicID, version, metadata != musicList.end() ? metadata->Title : sf::String()});

        return m_queue.size();
    }

    void MusicDownloaderService::Remove(const std::uint16_t musicID)
    {
        auto lock = std::lock_guard(m_mutex);
        m_queue.erase(std::remove_if(m_queue.begin(), m_queue.end(), [musicID](const DownloadItem& item)
        {
            return item.MusicID == musicID;
        }), m_queue.end());
    }

    bool MusicDownloaderService::Contains(const std::uint16_t musicID) const
    {
        auto lock = std::lock_guard(m_mutex);
        return std::any_of(m_queue.begin(), m_queue.end(), [musicID](const DownloadItem& item)
        {
            return item.MusicID == musicID;
        });
    }

    std::size_t MusicDownloaderService::GetQueueCount() const
    {
        auto lock = std::lock_guard(m_mutex);
        return m_queue.size();
    }

    void MusicDownloaderService::ClearQueue()
    {
        auto lock = std::lock_guard(m_mutex);
        m_queue.clear();
    }

    void MusicDownloaderService::StartDownload()
    {
        if (m_running)
            return;

        if (m_thread.joinable())
            m_thread.join();

        m_running   = true;
        m_cancelled = false;
        m_thread    = std::thread([this] { DownloadProc(); });
    }

    void MusicDownloaderService::Cancel()
    {
        m_cancelled = true;
    }

    bool MusicDownloaderService::IsDownloading() const
    {
        return m_running;
    }

    MusicDownloaderService::DownloadProgress MusicDownloaderService::GetProgress() const
    {
        auto lock = std::lock_guard(m_mutex);
        return m_progress;
    }

    void MusicDownloaderService::SetDownloadStartedCallback(const std::function<void(std::uint16_t)>& callback)
    {
        m_downloadStartedCallback = callback;
    }

    void MusicDownloaderService::SetRenamingCallback(const std::function<void(std::uint16_t)>& callback)
    {
        m_renamingCallback = callback;
    }

    void MusicDownloaderService::SetDownloadCompletedCallback(const std::function<void(std::uint16_t)>& callback)
    {
        m_downloadCompletedCallback = callback;
    }

    void MusicDownloaderService::SetQueueCompletedCallback(const std::function<void()>& callback)
    {
        m_queueCompletedCallback = callback;
    }

    void MusicDownloaderService::SetErrorCallback(const std::function<void(DownloadError)>& callback)
    {
        m_errorCallback = callback;
    }

    void MusicDownloaderService::Update(const sf::Time& delta)
    {
        auto notifications = std::vector<Notification>();
        {
            auto lock = std::lock_guard(m_mutex);
            notifications.swap(m_notifications);
        }

        for (const auto& notification : notifications)
        {
            switch (notification.Kind)
            {
                case Notification::Type::DownloadStarted:
                    if (m_downloadStartedCallback)
                        m_downloadStartedCallback(notification.MusicID);
                    break;

                case Notification::Type::Renaming:
                    if (m_renamingCallback)
                        m_renamingCallback(notification.MusicID);
                    break;

                case Notification::Type::DownloadCompleted:
                    if (m_downloadCompletedCallback)
                        m_downloadCompletedCallback(notification.MusicID);
                    break;

                case Notification::Type::QueueCompleted:
                    if (m_queueCompletedCallback)
                        m_queueCompletedCallback();
                    break;

                case Notification::Type::Error:
                    if (m_errorCallback)
                        m_errorCallback(notification.Error);
                    break;
            }
        }
    }

    void MusicDownloaderService::Fail(const DownloadError error)
    {
        auto lock = std::lock_guard(m_mutex);
        if (error == DownloadError::DownloadFailed || error == DownloadError::Cancelled)
            m_queue.clear();

        m_progress = DownloadProgress();
        m_progress.Status = error == DownloadError::Cancelled ? DownloadStatus::Idle : DownloadStatus::Failed;
        m_progress.Error  = error == DownloadError::Cancelled ? DownloadError::None : error;
        m_notifications.push_back({Notification::Type::Error, 0, error});

        m_cancelled = false;
        m_running   = false;
    }

    void MusicDownloaderService::DownloadProc()
    {
        auto items = std::vector<DownloadItem>();
        {
            auto lock = std::lock_guard(m_mutex);
            items = m_queue;
            m_progress = DownloadProgress();
            m_progress.Status = DownloadStatus::Connecting;
            m_progress.QueueCount = items.size();
        }

        if (items.empty())
        {
            auto lock = std::lock_guard(m_mutex);
            m_progress.Status = DownloadStatus::Completed;
            m_notifications.push_back({Notification::Type::QueueCompleted, 0, DownloadError::None});
            m_running = false;
            return;
        }

        auto url = m_args.GetFtpUrl();
        if (const auto separator = url.find('/'); separator != std::string::npos)
            url = url.substr(0, separator);

        auto port = std::optional<unsigned short>();
        if (const auto separator = url.rfind(':'); separator != std::string::npos)
        {
            try
            {
                port = static_cast<unsigned short>(std::stoi(url.substr(separator + 1)));
            }
            catch (...)
            {
                Fail(DownloadError::ConnectionFailed);
                return;
            }

            url = url.substr(0, separator);
        }

        auto address = std::optional<sf::IpAddress>();
        if (auto addresses = sf::Dns::resolve(url); addresses.has_value())
        {
            addresses->erase(
                std::remove_if(addresses->begin(), addresses->end(), [](const auto& entry)
                {
                    return !entry.isV4();
                }), addresses->end()
            );

            if (!addresses->empty())
                address = addresses->front();
        }

        if (!address.has_value())
        {
            Fail(DownloadError::ConnectionFailed);
            return;
        }

        auto sftp = std::optional<sf::Sftp>();
        auto ftp  = std::optional<sf::Ftp>();

        const auto remoteDir = std::filesystem::path(m_args.GetFtpPath()) / RemoteDirectory;
        const auto deadline  = [this](const sf::Time limit)
        {
            return sf::TimeoutWithPredicate([this, limit, clock = sf::Clock()]
            {
                return !m_cancelled && clock.getElapsedTime() < limit;
            }, PollingPeriod);
        };

        auto protocol = m_protocol.load();
        if (protocol != Protocol::Ftp && port.value_or(SftpPort) != FtpPort)
        {
            sftp.emplace();
            if (sftp->connect(*address, port.value_or(SftpPort), deadline(ConnectTimeout)).isOk() &&
                sftp->login(UserName, Password, deadline(ConnectTimeout)).isOk())
            {
                protocol = Protocol::Sftp;
            }
            else
            {
                sftp.reset();
            }
        }

        if (!sftp.has_value() && protocol != Protocol::Sftp)
        {
            ftp.emplace();
            if (ftp->connect(*address, port.value_or(FtpPort), ConnectTimeout).isOk() &&
                ftp->login(UserName, Password).isOk() &&
                ftp->changeDirectory(remoteDir.generic_string()).isOk() &&
                ftp->sendCommand("TYPE", "I").isOk())
            {
                protocol = Protocol::Ftp;
            }
            else
            {
                ftp.reset();
            }
        }

        if (!sftp.has_value() && !ftp.has_value())
        {
            Fail(m_cancelled ? DownloadError::Cancelled : DownloadError::ConnectionFailed);
            return;
        }

        m_protocol = protocol;
        const auto getRemoteSize = [&](const std::string& name) -> std::optional<std::uint64_t>
        {
            if (sftp.has_value())
            {
                auto result = sftp->getAttributes((remoteDir / name).generic_string(), true, deadline(ConnectTimeout));
                if (result.isOk())
                    return result.getAttributes().size;

                return std::nullopt;
            }

            const auto response = ftp->sendCommand("SIZE", name);
            if (response.getStatus() != sf::Ftp::Response::Status::FileStatus)
                return std::nullopt;

            try
            {
                return std::stoull(response.getMessage());
            }
            catch (...)
            {
                return std::nullopt;
            }
        };

        auto queue = std::vector<std::vector<FileEntry>>();
        std::uint64_t totalSize = 0;
        for (const auto& item : items)
        {
            const auto ojn = GetTempFileName(item.MusicID, item.Version) + ".ojn";
            const auto ojm = GetTempFileName(item.MusicID, 0) + ".ojm";

            const auto ojnSize = getRemoteSize(ojn);
            const auto ojmSize = getRemoteSize(ojm);
            if (!ojnSize.has_value() || !ojmSize.has_value())
            {
                Fail(DownloadError::ConnectionFailed);
                return;
            }

            totalSize += *ojnSize + *ojmSize;
            queue.push_back({{ojn, *ojnSize, 0}, {ojm, *ojmSize, 0}});
        }

        auto error = std::error_code();
        if (const auto space = std::filesystem::space(".", error); !error && space.available < totalSize)
        {
            Fail(DownloadError::InsufficientDiskSpace);
            return;
        }

        {
            auto lock = std::lock_guard(m_mutex);
            m_progress.Status = DownloadStatus::Downloading;
            m_progress.TotalSize = totalSize;
        }

        std::filesystem::create_directories(TempDirectory, error);

        auto clock = sf::Clock();
        std::uint64_t window = 0;

        for (std::size_t index = 0; index < items.size(); index++)
        {
            const auto& item = items[index];
            auto& entries = queue[index];

            const auto iniPath = std::filesystem::path(TempDirectory) / (GetTempFileName(item.MusicID, item.Version) + ".ini");

            {
                auto lock = std::lock_guard(m_mutex);
                m_progress.QueueIndex = index;
                m_progress.MusicID    = item.MusicID;
                m_progress.MusicTitle = item.Title;
                m_notifications.push_back({Notification::Type::DownloadStarted, item.MusicID, DownloadError::None});
            }

            WriteFileInfo(iniPath, entries);

            auto failure = std::optional<DownloadError>();
            for (std::size_t fileIndex = 0; fileIndex < entries.size(); fileIndex++)
            {
                auto& entry = entries[fileIndex];
                const auto tempPath  = std::filesystem::path(TempDirectory) / (entry.Name + "_");
                const auto localPath = std::filesystem::path(MusicDirectory) / entry.Name;

                {
                    auto lock = std::lock_guard(m_mutex);
                    m_progress.FileName = entry.Name;
                    m_progress.FileSize = entry.Size;
                    m_progress.FileBytesRead = 0;
                }

                if (m_cancelled)
                {
                    failure = DownloadError::Cancelled;
                    break;
                }

                if (fileIndex == 1)
                {
                    auto matchError = std::error_code();
                    if (std::filesystem::is_regular_file(localPath, matchError) &&
                        std::filesystem::file_size(localPath, matchError) == entry.Size &&
                        !matchError)
                    {
                        entry.Status = 4;
                        WriteFileInfo(iniPath, entries);

                        auto lock = std::lock_guard(m_mutex);
                        m_progress.FileBytesRead   = entry.Size;
                        m_progress.TotalBytesRead += entry.Size;
                        continue;
                    }
                }

                entry.Status = 0;
                WriteFileInfo(iniPath, entries);
                std::filesystem::remove(tempPath, error);

                bool success = false;
                if (sftp.has_value())
                {
                    if (auto file = std::ofstream(tempPath, std::ios::binary | std::ios::trunc))
                    {
                        auto stall = sf::Clock();
                        const auto result = sftp->download(
                            (remoteDir / entry.Name).generic_string(),
                            [&](const void* data, const std::size_t size)
                            {
                                if (m_cancelled)
                                    return false;

                                stall.restart();
                                file.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));

                                auto lock = std::lock_guard(m_mutex);
                                m_progress.FileBytesRead  += size;
                                m_progress.TotalBytesRead += size;

                                window += size;
                                if (clock.getElapsedTime() >= sf::seconds(1))
                                {
                                    m_progress.BytesPerSecond = window;
                                    window = 0;
                                    clock.restart();
                                }

                                return file.good();
                            },
                            0,
                            sf::TimeoutWithPredicate([&]
                            {
                                return !m_cancelled && stall.getElapsedTime() < TransferTimeout;
                            }, PollingPeriod)
                        );

                        const bool complete = file.good();
                        file.close();
                        success = result.isOk() && complete && !m_cancelled;
                    }
                }
                else
                {
                    std::filesystem::remove(std::filesystem::path(TempDirectory) / entry.Name, error);
                    success = ftp->download(entry.Name, TempDirectory).isOk() && !m_cancelled;
                    if (success)
                    {
                        std::filesystem::rename(std::filesystem::path(TempDirectory) / entry.Name, tempPath, error);
                        success = !error;

                        auto lock = std::lock_guard(m_mutex);
                        m_progress.FileBytesRead   = entry.Size;
                        m_progress.TotalBytesRead += entry.Size;
                    }
                }

                if (!success)
                {
                    failure = m_cancelled ? DownloadError::Cancelled : DownloadError::DownloadFailed;
                    break;
                }

                entry.Status = 4;
                WriteFileInfo(iniPath, entries);
            }

            if (failure.has_value())
            {
                for (const auto& entry : entries)
                {
                    auto cleanupError = std::error_code();
                    std::filesystem::remove(std::filesystem::path(TempDirectory) / (entry.Name + "_"), cleanupError);
                    std::filesystem::remove(std::filesystem::path(TempDirectory) / entry.Name, cleanupError);
                }

                std::filesystem::remove(iniPath, error);
                std::filesystem::remove(TempDirectory, error);

                Fail(*failure);
                return;
            }

            {
                auto lock = std::lock_guard(m_mutex);
                m_notifications.push_back({Notification::Type::Renaming, item.MusicID, DownloadError::None});
            }

            for (const auto& entry : entries)
            {
                const auto tempPath  = std::filesystem::path(TempDirectory) / (entry.Name + "_");
                const auto localPath = std::filesystem::path(MusicDirectory) / entry.Name;

                auto moveError = std::error_code();
                if (std::filesystem::exists(localPath, moveError))
                    std::filesystem::remove(tempPath, moveError);
                else
                    std::filesystem::rename(tempPath, localPath, moveError);
            }

            std::filesystem::remove(iniPath, error);

            {
                auto lock = std::lock_guard(m_mutex);
                m_notifications.push_back({Notification::Type::DownloadCompleted, item.MusicID, DownloadError::None});
            }
        }

        std::filesystem::remove(TempDirectory, error);

        {
            auto lock = std::lock_guard(m_mutex);
            m_queue.clear();
            m_progress = DownloadProgress();
            m_progress.Status = DownloadStatus::Completed;
            m_notifications.push_back({Notification::Type::QueueCompleted, 0, DownloadError::None});
        }

        m_cancelled = false;
        m_running   = false;
    }
}
