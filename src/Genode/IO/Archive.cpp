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

    std::optional<std::size_t> Archive::ReadFile(const FileInfo& entry, void* data) const
    {
        if (&entry.GetParent() != this)
            throw ResourceAccessException(entry.GetName(), "The specified file doesn't belong to this archive");

        return ReadFile(entry.GetName(), data, entry.GetSize());
    }

    const std::string& Archive::GetFileName() const
    {
        return m_filename;
    }

    std::vector<std::unique_ptr<FileInfo>> Archive::Scan(const std::string& pattern, bool recursive) const
    {
        std::vector<std::unique_ptr<FileInfo>> files;
        for (const auto& entry : GetFileEntries())
        {
            if (auto fileName = entry->GetName(); StringHelper::IsGlobMatch(fileName, pattern))
                files.push_back(std::make_unique<FileInfo>(*this, fileName, GetFileSize(fileName).value_or(0)));
        }

        return files;
    }
}
