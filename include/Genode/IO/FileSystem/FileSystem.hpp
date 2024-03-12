#ifndef GENODE_IO_FILESYSTEM_HPP
#define GENODE_IO_FILESYSTEM_HPP

#include <Genode/System/Primitives.hpp>
#include <Genode/IO/Resource.hpp>

#include <SFML/System/InputStream.hpp>

#include <memory>
#include <vector>
#include <string>

namespace Gx
{
    class FileInfo;
    class FileSystemController;
    class FileSystem
    {
        public:
            FileSystem() = delete;
            ~FileSystem() = delete;

            static ResourcePtr<sf::InputStream> Open(const std::string &fileName);

            static bool Contains(std::string &fileName);
            static std::unique_ptr<FileInfo> GetFileInfo(const std::string &fileName);

            static Int64 ReadFile(const std::string &fileName, void *data, Int64 size);
            static std::size_t GetFileSize(const std::string &fileName);

            static void Mount(const FileSystemController &fileSystem);
            static void Dismount(const FileSystemController &fileSystem);

        private:
            using FileSystemMap = std::vector<const FileSystemController*>;

            inline static FileSystemMap m_controllers;
    };



}

#endif
