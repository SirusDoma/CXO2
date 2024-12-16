#include <Genode/IO/FileSystem/FileSystemController.hpp>

namespace Gx
{
    std::vector<std::unique_ptr<FileInfo>> FileSystemController::Scan(const std::string& pattern, bool recursive) const
    {
        return {};
    }

    const std::string& FileSystemController::GetPrefix() const
    {
        return m_prefix;
    }

    void FileSystemController::SetPathPrefix(const std::string& prefix)
    {
        m_prefix = prefix;
    }
}