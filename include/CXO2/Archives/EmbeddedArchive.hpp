#pragma once

#include <CXO2/Utilities/CaseInsensitiveComparator.hpp>
#include <Genode/IO/Archive.hpp>

#include <map>
#include <filesystem>

namespace Cx
{
    class EmbeddedArchive final : public virtual Gx::Archive
    {
    public:
        EmbeddedArchive() = default;

        [[nodiscard]] Gx::ResourcePtr<sf::InputStream> Open(const std::filesystem::path& fileName) const override;

        [[nodiscard]] bool Contains(const std::filesystem::path& fileName) const override;

        [[nodiscard]] std::unique_ptr<Gx::FileInfo> GetFileInfo(const std::filesystem::path& fileName) const override;

        [[nodiscard]] std::vector<std::unique_ptr<Gx::FileInfo>> GetFileEntries() const override;

        std::optional<std::size_t> ReadFile(const std::filesystem::path& name, void* data, std::size_t size) const override;

        void WriteFile(const std::filesystem::path& fileName, const void* data, std::size_t size) override;

        void WriteFile(const std::filesystem::path& fileName, const std::vector<std::uint8_t>& bytes);

        [[nodiscard]] std::optional<std::size_t> GetFileSize(const std::filesystem::path& fileName) const override;

    private:
        struct BlobData
        {
            const void* Data = nullptr;
            std::size_t Size = 0;
        };

        std::map<std::string, BlobData, CaseInsensitiveComparator> m_entries;
    };
}
