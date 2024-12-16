#pragma once

#include <Genode/IO/FileSystem/FileSystemController.hpp>

#include <Genode/System/Exception.hpp>

#include <vector>
#include <string>

namespace Gx
{
    class LocalFileSystem final : public FileSystemController
    {
    public:
        static LocalFileSystem& Instance();

        static std::string GetApplicationDirectoryPath();

        static std::string GetWorkingDirectory();
        static void SetWorkingDirectory(const std::string& inputPath);

        static std::vector<std::string> GetAssetPaths();
        static void AddAssetPath(const std::string& path);

        [[nodiscard]] std::string GetFileName(const std::string& fullPath, bool withExtension = true) const;
        [[nodiscard]] std::string GetFullName(const std::string& fileName, bool withExtension = true) const;

        [[nodiscard]] ResourcePtr<sf::InputStream> Open(const std::string& fileName) const override;

        [[nodiscard]] bool Contains(const std::string& fileName) const override;

        [[nodiscard]] std::unique_ptr<FileInfo> GetFileInfo(const std::string& fileName) const override;

        [[nodiscard]] std::vector<std::unique_ptr<FileInfo>> GetFileEntries() const override { throw Gx::NotSupportedException(); }

        std::optional<std::size_t> ReadFile(const std::string& fileName, void* data, std::size_t size) const override;
        void WriteFile(const std::string& fileName, const void* data, std::size_t size) override;

        [[nodiscard]] std::optional<std::size_t> GetFileSize(const std::string& fileName) const override;

        [[nodiscard]] std::vector<std::unique_ptr<FileInfo>> Scan(const std::string& pattern, bool recursive) const override;

    private:
        LocalFileSystem() = default;

        inline static std::vector<std::string> m_paths;
    };
}
