#ifndef GENODE_IO_FILE_INFO_HPP
#define GENODE_IO_FILE_INFO_HPP


#include <string>

namespace Gx
{
    class FileSystemController;
    class FileInfo
    {
    public:
        FileInfo() = default;
        FileInfo(const FileSystemController& parent, const std::string& name, std::int64_t size);
        virtual ~FileInfo() = default;

        const FileSystemController& GetParent() const;
        std::string GetName() const;
        std::uint32_t GetSize() const;

        virtual std::int64_t Read(void* data) const;

    private:
        const FileSystemController* m_parent = nullptr;
        std::string m_name;
        std::int64_t m_size = 0;
    };
}

#endif
