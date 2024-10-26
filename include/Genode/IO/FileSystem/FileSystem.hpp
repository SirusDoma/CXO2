#pragma once

#include <Genode/IO/Resource.hpp>

#include <SFML/System/InputStream.hpp>

#include <memory>
#include <vector>
#include <string>

namespace Gx
{
    class FileInfo;
    class FileSystemController;
    class FileSystem final
    {
        public:
            FileSystem() = delete;
            ~FileSystem() = delete;

            static ResourcePtr<sf::InputStream> Open(const std::string& fileName);

            static std::vector<std::unique_ptr<FileInfo>> Scan(const std::string& pattern);

            static bool Contains(const std::string& fileName);
            static std::unique_ptr<FileInfo> GetFileInfo(const std::string& fileName);

            static std::optional<std::size_t> ReadFile(const std::string& fileName, void* data, std::size_t size);
            static std::optional<std::size_t> GetFileSize(const std::string& fileName);

            static void Mount(const FileSystemController& fileSystem);
            static void Dismount(const FileSystemController& fileSystem);

        private:
            using FileSystemMap = std::vector<const FileSystemController*>;

            inline static FileSystemMap m_controllers;
    };

}
