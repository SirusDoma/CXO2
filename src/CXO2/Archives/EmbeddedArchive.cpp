#include <CXO2/Archives/EmbeddedArchive.hpp>

#include <SFML/System/MemoryInputStream.hpp>

#include <algorithm>
#include <cstring>

namespace Cx
{
    Gx::ResourcePtr<sf::InputStream> EmbeddedArchive::Open(const std::string& fileName) const
    {
        if (const auto it = m_entries.find(fileName); it != m_entries.end())
        {
            std::unique_ptr<sf::InputStream> stream = std::make_unique<sf::MemoryInputStream>(it->second.Data, it->second.Size);
            return stream;
        }

        return nullptr;
    }

    bool EmbeddedArchive::Contains(const std::string& fileName) const
    {
        return m_entries.find(fileName) != m_entries.end();
    }

    std::unique_ptr<Gx::FileInfo> EmbeddedArchive::GetFileInfo(const std::string& fileName) const
    {
        if (const auto it = m_entries.find(fileName); it != m_entries.end())
            return std::make_unique<Gx::FileInfo>(*this, it->first, it->second.Size);

        return nullptr;
    }

    std::vector<std::unique_ptr<Gx::FileInfo>> EmbeddedArchive::GetFileEntries() const
    {
        auto entries = std::vector<std::unique_ptr<Gx::FileInfo>>();
        for (const auto& [name, blob] : m_entries)
            entries.push_back(std::make_unique<Gx::FileInfo>(*this, name, blob.Size));

        return entries;
    }

    std::optional<std::size_t> EmbeddedArchive::ReadFile(const std::string& name, void* data, std::size_t size) const
    {
        if (const auto it = m_entries.find(name); it != m_entries.end())
        {
            std::memcpy(data, it->second.Data, it->second.Size);
            return it->second.Size;
        }

        return std::nullopt;
    }

    void EmbeddedArchive::WriteFile(const std::string& fileName, const void* data, const std::size_t size)
    {
        m_entries[fileName] = BlobData{ data, size };
    }

    void EmbeddedArchive::WriteFile(const std::string& fileName, const std::vector<std::uint8_t>& bytes)
    {
        m_entries[fileName] = BlobData{ bytes.data(), bytes.size() };
    }

    std::optional<std::size_t> EmbeddedArchive::GetFileSize(const std::string& fileName) const
    {
        if (const auto it = m_entries.find(fileName); it != m_entries.end())
            return it->second.Size;

        return std::nullopt;
    }
}
