#pragma once

#include <optional>
#include <string>

namespace Gx
{
    class FileSystemController;
    class FileInfo
    {
    public:
        FileInfo() = default;
        FileInfo(const FileSystemController& parent, const std::string& name, std::size_t size);
        virtual ~FileInfo() = default;

        const FileSystemController& GetParent() const;
        std::string GetName() const;
        std::size_t GetSize() const;

        virtual std::optional<std::size_t> Read(void* data) const;

    private:
        const FileSystemController* m_parent = nullptr;
        std::string m_name;
        std::size_t m_size = 0;
    };
}
