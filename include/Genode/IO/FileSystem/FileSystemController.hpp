#pragma once

#include <Genode/IO/Resource.hpp>
#include <Genode/IO/FileSystem/FileInfo.hpp>

#include <SFML/System/InputStream.hpp>

#include <optional>
#include <string>
#include <vector>
#include <memory>

namespace Gx
{
    class FileSystemController
    {
    public:
        virtual ~FileSystemController() = default;

        virtual ResourcePtr<sf::InputStream> Open(const std::string& fileName) const = 0;

        virtual std::vector<std::unique_ptr<FileInfo>> Scan(const std::string& pattern, bool recursive) const;
        virtual bool Contains(const std::string& fileName) const = 0;

        virtual std::unique_ptr<FileInfo> GetFileInfo(const std::string& fileName) const = 0;
        virtual std::vector<std::unique_ptr<FileInfo>> GetFileEntries() const = 0;

        virtual std::optional<std::size_t> ReadFile(const std::string& fileName, void* data, std::size_t size) const = 0;
        virtual void WriteFile(const std::string& fileName, const void* data, std::size_t size) = 0;

        virtual std::optional<std::size_t> GetFileSize(const std::string& fileName) const = 0;

        const std::string& GetPrefix() const;
        void SetPathPrefix(const std::string& prefix);

    private:
        std::string m_prefix;
    };
}
