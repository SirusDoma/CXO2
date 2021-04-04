#ifndef GENODE_IO_ARCHIVE_HPP
#define GENODE_IO_ARCHIVE_HPP

#include <Genode/IO/FileSystem.hpp>
#include <Genode/System/Primitives.hpp>

#include <string>
#include <vector>

namespace Gx
{
    class Archive : public FileSystem<Archive>
    {
    public:
        friend class ResourceManager;

        Archive();
        virtual ~Archive();

        std::string GetFileName() const;
        virtual bool Open(const std::string& fileName);

        virtual Int64 GetFile(const std::string& name, Uint8** data) const = 0;
        virtual std::vector<FileEntry> GetFileEntries() const = 0;
        
    private:
        std::string m_filename;
    };
}

#endif