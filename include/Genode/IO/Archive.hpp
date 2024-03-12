#ifndef GENODE_IO_ARCHIVE_HPP
#define GENODE_IO_ARCHIVE_HPP

#include <string>
#include <vector>

#include <Genode/System/Primitives.hpp>
#include <Genode/IO/Resource.hpp>
#include <Genode/IO/FileSystem/FileSystemController.hpp>

namespace Gx
{
    class Archive : public FileSystemController
    {
    public:

        Archive() = default;
        ~Archive() override = default;

        std::string GetFileName() const;

        virtual bool LoadFromFile(const std::string& fileName);

        ResourcePtr<sf::InputStream> Open(const std::string &fileName) const override = 0;

        bool Contains(const std::string& fileName) const override = 0;
        std::unique_ptr<FileInfo> GetFileInfo(const std::string &fileName) const override = 0;
        std::vector<FileInfo> GetFileEntries() const override = 0;

        Int64 ReadFile(const std::string& name, void *data, Int64 size) const override = 0;
        virtual Int64 ReadFile(const FileInfo &entry, void *data) const;

        Int64 GetFileSize(const std::string &fileName) const override = 0;

    private:
        std::string m_filename;
    };
}

#endif