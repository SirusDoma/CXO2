#include <Genode/IO/FileSystem/FileInfo.hpp>

#include <Genode/IO/FileSystem/FileSystemController.hpp>

namespace Gx
{
    FileInfo::FileInfo(const FileSystemController &parent, const std::string &name, const Int64 size) :
        m_parent(&parent),
        m_name(name),
        m_size(size)
    {
    }

    const FileSystemController &FileInfo::GetParent() const
    {
        return *m_parent;
    }

    std::string FileInfo::GetName() const
    {
        return m_name;
    }

    Uint32 FileInfo::GetSize() const
    {
        return m_size;
    }

    Int64 FileInfo::Read(void *data) const
    {
        return m_parent->ReadFile(m_name, data, m_size);
    }
}
