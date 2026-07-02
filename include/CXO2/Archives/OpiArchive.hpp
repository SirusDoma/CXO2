#pragma once

#include <CXO2/Archives/FileInfo.hpp>
#include <CXO2/Utilities/CaseInsensitiveComparator.hpp>

#include <Genode/IO.hpp>
#include <SFML/System/FileInputStream.hpp>

#include <mutex>
#include <map>
#include <filesystem>

namespace Cx
{
    struct OpiItemHeader
    {
        char          Name[128];
        std::uint32_t Reference;
        std::uint32_t Size1;
        std::uint32_t Size2;
        std::uint32_t Unused1;
        std::uint32_t Unused2;

        std::uint32_t GetSize() const { return std::max(Size1, Size2); }
    };

    class OpiArchive final : public virtual Gx::Archive
    {
    public:
        enum class Signature : std::uint32_t
        {
            Unknown,
            OPA = 01,
            OPI = 02,
        };

        OpiArchive() = default;

        Signature GetSignature() const;
        bool LoadFromFile(const std::filesystem::path& fileName) override;

        Gx::ResourcePtr<sf::InputStream> Open(const std::filesystem::path& fileName) const override;

        bool Contains(const std::filesystem::path& name) const override;
        std::vector<std::unique_ptr<Gx::FileInfo>> GetFileEntries() const override;
        std::unique_ptr<Gx::FileInfo> GetFileInfo(const std::filesystem::path& fileName) const override;

        std::optional<std::size_t> ReadFile(const std::filesystem::path& fileName, void* data, std::size_t size) const override;
        void WriteFile(const std::filesystem::path& fileName, const void* data, std::size_t size) override { throw Gx::NotSupportedException(); }

        std::optional<std::size_t> GetFileSize(const std::filesystem::path& fileName) const override;

    private:
        const unsigned int ITEM_HEADER_SIZE = 152;

        bool ReadStream(void* data, std::uint64_t size) const;

        std::string m_source;
        Signature m_signature;
        std::uint32_t m_count;

        mutable std::map<std::string, FileInfo, CaseInsensitiveComparator> m_entries;
        mutable std::mutex m_mutex;
        mutable sf::FileInputStream m_fileStream;
    };
}
