#ifndef GENODE_IO_FILE_INFO_HPP
#define GENODE_IO_FILE_INFO_HPP

#include <Genode/System/Primitives.hpp>

#include <string>

namespace Gx
{
    class FileSystemController;
    class FileInfo
    {
    public:
        FileInfo() = default;
        FileInfo(const FileSystemController& parent, const std::string& name, Int64 size);
        virtual ~FileInfo() = default;

        const FileSystemController& GetParent() const;
        std::string GetName() const;
        Uint32 GetSize() const;

        virtual Int64 Read(void* data) const;

    private:
        const FileSystemController* m_parent = nullptr;
        std::string m_name;
        Int64 m_size = 0;
    };
}

#endif
