#include <Genode/IO/Archive.hpp>
#include <Genode/IO/FileSystem.hpp>

namespace Gx
{
    Archive::Archive() :
        m_filename()
    {
    }

    Archive::~Archive()
    {
    }

    bool Archive::Open(const std::string& fileName)
    {
        m_filename = fileName;
        return true;
    }

    std::string Archive::GetFileName() const
    {
        return m_filename;
    }
}