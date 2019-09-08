#ifndef GENODE_ARCHIVE_HPP
#define GENODE_ARCHIVE_HPP

#include <Genode/IO/FileSystem.hpp>
#include <Genode/System/Primitives.hpp>

#include <string>
#include <vector>

namespace Gx
{
    class Archive : public FileSystem<Archive>
    {
    public:
        friend class Cache;
        friend class ResourceManager;

        std::string GetFileName() const;
        virtual bool Open(const std::string& fileName);

        virtual Int64 GetFile(const std::string& name, Uint8** data) const = 0;
        virtual std::vector<FileEntry> GetFileEntries() const = 0;

    protected:
        Archive();
        virtual ~Archive();
        
    private:
        std::string m_filename;
    };
}

#endif