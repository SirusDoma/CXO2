#ifndef GENODE_IO_ARCHIVE_HPP
#define GENODE_IO_ARCHIVE_HPP

#include <Genode/IO/Resource.hpp>
#include <Genode/IO/FileSystem/FileSystemController.hpp>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace Gx
{
    class Archive : public FileSystemController
    {
    public:

        Archive() = default;
        ~Archive() override = default;

        const std::string& GetFileName() const;

        virtual bool LoadFromFile(const std::string& fileName);

        ResourcePtr<sf::InputStream> Open(const std::string& fileName) const override = 0;

        std::vector<std::unique_ptr<FileInfo>> Scan(const std::string& pattern, bool recursive) const override;
        bool Contains(const std::string& fileName) const override = 0;

        std::unique_ptr<FileInfo> GetFileInfo(const std::string& fileName) const override = 0;

        std::vector<std::unique_ptr<FileInfo>> GetFileEntries() const override = 0;

        std::int64_t ReadFile(const std::string& name, void* data, std::int64_t size) const override = 0;
        virtual std::int64_t ReadFile(const FileInfo& entry, void* data) const;

        std::int64_t GetFileSize(const std::string& fileName) const override = 0;

    private:
        std::string m_filename;
    };
}

#endif