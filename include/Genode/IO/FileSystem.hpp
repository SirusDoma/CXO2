#ifndef GENODE_IO_FILESYSTEM_HPP
#define GENODE_IO_FILESYSTEM_HPP

#include <Genode/System/Primitives.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <vector>
#include <string>

namespace Gx
{
    template<typename T>
    class FileSystem
    {
    public:
        struct FileEntry
        {
            const T* Parent = nullptr;
            std::string Name;
            Gx::Uint32  Size = 0;

            virtual ~FileEntry() {};
            virtual Int64 GetContent(Uint8** data) { return Parent->GetFile(Name, data); }
        };

        FileSystem() {}
        virtual ~FileSystem() {}

        virtual bool Contains(const std::string& fileName) const = 0;
        virtual Int64 GetFile(const std::string& filename, Uint8** data) const = 0;
        virtual std::vector<FileEntry> GetFileEntries() const = 0;
    };
}

#endif