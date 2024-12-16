#include <Genode/IO/FileSystem/FileInfo.hpp>

#include <Genode/IO/FileSystem/FileSystemController.hpp>
#include <utility>

namespace Gx
{
    FileInfo::FileInfo(const FileSystemController& parent, std::string  name, const std::size_t size) :
        m_parent(&parent),
        m_name(std::move(name)),
        m_size(size)
    {
    }

    const FileSystemController& FileInfo::GetParent() const
    {
        return *m_parent;
    }

    std::string FileInfo::GetName() const
    {
        return m_name;
    }

    std::size_t FileInfo::GetSize() const
    {
        return m_size;
    }

    std::optional<std::size_t> FileInfo::Read(void* data) const
    {
        return m_parent->ReadFile(m_name, data, m_size);
    }
}
