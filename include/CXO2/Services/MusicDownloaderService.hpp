#pragma once
#include <CXO2/Services/Service.hpp>
#include <CXO2/Models/Shop.hpp>

#include <Genode/Entities/Updatable.hpp>
#include <Genode/System/Module.hpp>

#include <SFML/System/String.hpp>
#include <SFML/System/Time.hpp>

#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace Cx
{
    class CommandLineContext;
    class SessionContext;
    class NetworkService;

    class MusicDownloaderService : public virtual Service, public Gx::Module, public Gx::Updatable
    {
    public:
        MusicDownloaderService(CommandLineContext& args, SessionContext& session);
        ~MusicDownloaderService() override;

        std::size_t Enqueue(std::uint16_t musicID, std::uint16_t version = 0);
        void Remove(std::uint16_t musicID);
        bool Contains(std::uint16_t musicID) const;
        std::size_t GetQueueCount() const;
        void ClearQueue();

        void StartDownload();
        void Cancel();
        bool IsDownloading() const;

        MusicDownloadProgress GetProgress() const;

        void SetDownloadStartedCallback(const std::function<void(std::uint16_t)>& callback);
        void SetRenamingCallback(const std::function<void(std::uint16_t)>& callback);
        void SetDownloadCompletedCallback(const std::function<void(std::uint16_t)>& callback);
        void SetQueueCompletedCallback(const std::function<void()>& callback);
        void SetErrorCallback(const std::function<void(MusicDownloadError)>& callback);
        void SetProgressCallback(const std::function<void(const MusicDownloadProgress&)>& callback);

        void Update(const sf::Time& delta) override;

    private:
        enum class Protocol
        {
            Unknown,
            Sftp,
            Ftp
        };

        struct DownloadItem
        {
            std::uint16_t MusicID;
            std::uint16_t Version;
            sf::String Title;
        };

        struct Notification
        {
            enum class Type
            {
                DownloadStarted,
                Renaming,
                DownloadCompleted,
                QueueCompleted,
                Error
            };

            Type Kind;
            std::uint16_t MusicID;
            MusicDownloadError Error;
        };

        void DownloadProc();
        void Fail(MusicDownloadError error);

        CommandLineContext& m_args;
        SessionContext& m_session;

        std::vector<DownloadItem> m_queue;
        std::vector<Notification> m_notifications;
        MusicDownloadProgress m_progress;
        MusicDownloadProgress m_lastProgress;

        std::function<void(std::uint16_t)> m_downloadStartedCallback;
        std::function<void(std::uint16_t)> m_renamingCallback;
        std::function<void(std::uint16_t)> m_downloadCompletedCallback;
        std::function<void()> m_queueCompletedCallback;
        std::function<void(MusicDownloadError)> m_errorCallback;
        std::function<void(const MusicDownloadProgress&)> m_progressCallback;

        std::thread m_thread;
        mutable std::mutex m_mutex;
        std::atomic<bool> m_running{false};
        std::atomic<bool> m_cancelled{false};
        std::atomic<Protocol> m_protocol{Protocol::Unknown};
    };
}
