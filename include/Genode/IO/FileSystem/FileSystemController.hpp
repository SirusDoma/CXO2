#ifndef GENODE_IO_FILESYSTEM_CONTROLLER_HPP
#define GENODE_IO_FILESYSTEM_CONTROLLER_HPP

#include <Genode/System/NonCopyable.hpp>
#include <Genode/System/Primitives.hpp>
#include <Genode/IO/Resource.hpp>
#include <Genode/IO/FileSystem/FileInfo.hpp>

#include <SFML/System/InputStream.hpp>

#include <string>
#include <vector>
#include <memory>

namespace Gx
{
    class FileSystemController
    {
    public:
        virtual ~FileSystemController() = default;

        virtual ResourcePtr<sf::InputStream> Open(const std::string &fileName) const = 0;

        virtual bool Contains(const std::string &fileName) const = 0;

        virtual std::unique_ptr<FileInfo> GetFileInfo(const std::string &fileName) const = 0;
        virtual std::vector<FileInfo> GetFileEntries() const = 0;

        virtual Int64 ReadFile(const std::string &fileName, void *data, Int64 size) const = 0;
        virtual void WriteFile(const std::string &fileName, void *data, Int64 size) const = 0;

        virtual Int64 GetFileSize(const std::string &fileName) const = 0;

        const std::string &GetPrefix() const { return m_prefix; }
        void SetPathPrefix(const std::string &prefix) { m_prefix = prefix; }

    private:
        std::string m_prefix;
    };
}

#endif
