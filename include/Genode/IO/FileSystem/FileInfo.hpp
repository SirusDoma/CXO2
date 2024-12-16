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
        FileInfo(const FileSystemController& parent, std::string  name, std::size_t size);
        virtual ~FileInfo() = default;

        [[nodiscard]] const FileSystemController& GetParent() const;
        [[nodiscard]] std::string GetName() const;
        [[nodiscard]] std::size_t GetSize() const;

        virtual std::optional<std::size_t> Read(void* data) const;

    private:
        const FileSystemController* m_parent = nullptr;
        std::string m_name;
        std::size_t m_size = 0;
    };
}
