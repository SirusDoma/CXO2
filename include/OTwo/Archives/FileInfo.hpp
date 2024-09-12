#ifndef O2JAM_FILE_ENTRY_HPP
#define O2JAM_FILE_ENTRY_HPP

#include <Genode/IO/FileSystem/FileInfo.hpp>

class FileInfo final : public Gx::FileInfo
{
public:
    FileInfo() = default;

    FileInfo(const Gx::FileSystemController& parent, const std::string& name, const std::int64_t size, const std::uint16_t index, const std::uint64_t offset) :
        Gx::FileInfo(parent, name, size),
        m_index(index),
        m_offset(offset)
    {
    }

    std::uint16_t GetIndex() const { return m_index; }
    std::uint64_t GetOffset() const { return m_offset; }

private:
    std::uint16_t m_index  = 0;
    std::uint64_t m_offset = 0;
};

#endif
