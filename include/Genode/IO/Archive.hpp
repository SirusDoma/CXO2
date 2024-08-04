#ifndef GENODE_IO_ARCHIVE_HPP
#define GENODE_IO_ARCHIVE_HPP

#include <Genode/System/Primitives.hpp>
#include <Genode/IO/Resource.hpp>
#include <Genode/IO/FileSystem/FileSystemController.hpp>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace Gx
{
    class Archive : public virtual FileSystemController
    {
    public:

        Archive() = default;
        ~Archive() override = default;

        const std::string &GetFileName() const;

        virtual bool LoadFromFile(const std::string& fileName);

        ResourcePtr<sf::InputStream> Open(const std::string &fileName) const override = 0;

        std::vector<std::unique_ptr<FileInfo>> Scan(const std::string &pattern, bool recursive) const override;
        bool Contains(const std::string& fileName) const override = 0;

        std::unique_ptr<FileInfo> GetFileInfo(const std::string &fileName) const override = 0;

        std::vector<std::unique_ptr<FileInfo>> GetFileEntries() const override = 0;

        Int64 ReadFile(const std::string& name, void *data, Int64 size) const override = 0;
        virtual Int64 ReadFile(const FileInfo &entry, void *data) const;

        Int64 GetFileSize(const std::string &fileName) const override = 0;

    private:
        std::string m_filename;
    };
}

#endif