#ifndef O2JAM_FILE_ENTRY_HPP
#define O2JAM_FILE_ENTRY_HPP

#include <Genode/IO/FileSystem/FileInfo.hpp>

class FileInfo final : public Gx::FileInfo
{
public:
    FileInfo() = default;

    FileInfo(const Gx::FileSystemController& parent, const std::string& name, const Gx::Int64 size, const Gx::Uint16 index, const Gx::Uint64 offset) :
        Gx::FileInfo(parent, name, size),
        m_index(index),
        m_offset(offset)
    {
    }

    Gx::Uint16 GetIndex() const { return m_index; }
    Gx::Uint64 GetOffset() const { return m_offset; }

private:
    Gx::Uint16 m_index  = 0;
    Gx::Uint64 m_offset = 0;
};

#endif
