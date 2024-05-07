#ifndef GENODE_IO_LOCAL_FILESYSTEM_HPP
#define GENODE_IO_LOCAL_FILESYSTEM_HPP

#include <Genode/IO/FileSystem/FileSystemController.hpp>

#include <Genode/System/Primitives.hpp>
#include <Genode/System/Exception.hpp>

#include <vector>
#include <string>

namespace Gx
{
    class LocalFileSystem : public FileSystemController
    {
    public:
        static LocalFileSystem &Instance();

        static std::string GetApplicationDirectoryPath();

        static std::string GetWorkingDirectory();
        static void SetWorkingDirectory(const std::string &inputPath);

        static std::vector<std::string> GetAssetPaths();
        static void AddAssetPath(const std::string& path);

        std::string GetFileName(const std::string& fullPath, bool withExtension = true) const;
        std::string GetFullName(const std::string& fileName, bool withExtension = true) const;

        ResourcePtr<sf::InputStream> Open(const std::string &fileName) const override;

        bool Contains(const std::string &fileName) const override;

        std::unique_ptr<Gx::FileInfo> GetFileInfo(const std::string &fileName) const override;
        std::vector<FileInfo> GetFileEntries() const override { throw Gx::NotSupportedException(); }

        Int64 ReadFile(const std::string &fileName, void *data, Int64 size) const override;
        void WriteFile(const std::string &fileName, void *data, Int64 size) override;

        Int64 GetFileSize(const std::string &fileName) const override;

        std::vector<FileInfo> Scan(const std::string &pattern, bool recursive) const override;

    private:
        LocalFileSystem() = default;

        inline static std::vector<std::string> m_paths;
    };
}

#endif