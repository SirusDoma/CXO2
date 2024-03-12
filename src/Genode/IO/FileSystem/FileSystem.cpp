#include <Genode/IO/FileSystem/FileSystem.hpp>

#include <Genode/IO/IOException.hpp>
#include <Genode/IO/FileSystem/FileSystemController.hpp>
#include <Genode/IO/FileSystem/LocalFileSystem.hpp>

namespace
{
    void EnsureDefaultFileSystemsRegistered()
    {
        static bool registered = false;
        if (!registered)
        {
            Gx::FileSystem::Mount(Gx::LocalFileSystem::Instance());

            registered = true;
        }
    }
}

namespace Gx
{

    ResourcePtr<sf::InputStream> FileSystem::Open(const std::string &fileName)
    {
        EnsureDefaultFileSystemsRegistered();

        for (auto const& controller : m_controllers)
        {
            auto name = fileName;
            if (!controller->GetPrefix().empty() && fileName.compare(0, controller->GetPrefix().size(), controller->GetPrefix()) == 0)
                name = name.substr(controller->GetPrefix().size());

            if (controller->Contains(name))
                return controller->Open(name);
        }

        throw ResourceAccessException(fileName, "File is not exists or not supported.");
    }

    bool FileSystem::Contains(std::string &fileName)
    {
        EnsureDefaultFileSystemsRegistered();

        for (auto const& controller : m_controllers)
        {
            if (controller->Contains(fileName))
                return true;
        }

        return false;
    }

    std::unique_ptr<FileInfo> FileSystem::GetFileInfo(const std::string &fileName)
    {
        EnsureDefaultFileSystemsRegistered();

        for (auto const& controller : m_controllers)
        {
            if (controller->Contains(fileName))
                return controller->GetFileInfo(fileName);
        }

        throw ResourceAccessException(fileName, "File is not exists or not supported.");
    }

    Int64 FileSystem::ReadFile(const std::string &fileName, void *data, const Int64 size)
    {
        EnsureDefaultFileSystemsRegistered();

        for (auto const& controller : m_controllers)
        {
            auto name = fileName;
            if (!controller->GetPrefix().empty() && fileName.compare(0, controller->GetPrefix().size(), controller->GetPrefix()) == 0)
                name = name.substr(controller->GetPrefix().size());

            if (controller->Contains(name))
                return controller->ReadFile(name, data, size);
        }

        throw ResourceAccessException(fileName, "File is not exists or not supported.");
    }

    std::size_t FileSystem::GetFileSize(const std::string &fileName)
    {
        EnsureDefaultFileSystemsRegistered();

        for (auto const& controller : m_controllers)
        {
            auto name = fileName;
            if (!controller->GetPrefix().empty() && fileName.compare(0, controller->GetPrefix().size(), controller->GetPrefix()) == 0)
                name = name.substr(controller->GetPrefix().size());

            if (controller->Contains(name))
                return controller->GetFileSize(name);
        }

        throw ResourceAccessException(fileName, "File is not exists or not supported.");
    }

    void FileSystem::Mount(const FileSystemController &fileSystem)
    {
        m_controllers.push_back(&fileSystem);
    }

    void FileSystem::Dismount(const FileSystemController &fileSystem)
    {
        if (const auto it = std::find(m_controllers.begin(), m_controllers.end(), &fileSystem); it != m_controllers.end())
            m_controllers.erase(it);
    }
}
