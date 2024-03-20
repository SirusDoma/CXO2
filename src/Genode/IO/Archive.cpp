#include <Genode/IO/Archive.hpp>
#include <Genode/IO/IOException.hpp>
#include <Genode/IO/FileSystem/FileInfo.hpp>
#include <Genode/Utilities/StringHelper.hpp>

namespace Gx
{
    bool Archive::LoadFromFile(const std::string& fileName)
    {
        m_filename = fileName;
        SetPathPrefix(StringHelper::RemoveExtension(fileName) + "/");

        return true;
    }

    Int64 Archive::ReadFile(const FileInfo &entry, void *data) const
    {
        if (&entry.GetParent() != this)
            throw ResourceAccessException(entry.GetName(), "The specified file doesn't belong to this archive.");

        return ReadFile(entry.GetName(), data, entry.GetSize());
    }

    const std::string & Archive::GetFileName() const
    {
        return m_filename;
    }
}
