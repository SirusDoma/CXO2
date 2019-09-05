#ifndef GENODE_ARCHIVE_HPP
#define GENODE_ARCHIVE_HPP

#include <Genode/System/Primitives.hpp>
#include <string>
#include <vector>

namespace Gx
{
    class Archive
    {
    public:
        friend class ResourceContainer;
        struct FileEntry
        {
			Archive*    Parent;
            std::string Name;
            Gx::Uint32  Size = 0;

			virtual Int64 GetFile(Uint8** data) const;
        };

        virtual bool Open(const std::string& fileName);
        virtual bool Contains(const std::string& name) const = 0;
        
		std::string GetFileName() const;
		virtual Int64 GetFile(const std::string& name, Uint8** data) const = 0;

		virtual std::vector<FileEntry> GetFileEntries() = 0;

    protected:
        Archive();
        virtual ~Archive();

		virtual Int64 GetFile(const Archive::FileEntry* entry, Uint8** data) const = 0;
        
    private:
        std::string m_filename;
    };
}

#endif