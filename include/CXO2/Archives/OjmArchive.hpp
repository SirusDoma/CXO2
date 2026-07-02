#pragma once

#include <CXO2/Archives/FileInfo.hpp>
#include <CXO2/Archives/M30Archive.hpp>
#include <CXO2/Archives/OmcArchive.hpp>

#include <Genode/IO/Archive.hpp>
#include <Genode/System/Exception.hpp>

#include <SFML/System/FileInputStream.hpp>

#include <cstdint>
#include <unordered_map>
#include <filesystem>

namespace Cx
{
    class OjmArchive final : public virtual Gx::Archive, M30Archive, OmcArchive
    {
    public:
        OjmArchive() = default;

        bool LoadFromFile(const std::filesystem::path& fileName) override;

        Gx::ResourcePtr<sf::InputStream> Open(unsigned int index) const override;
        Gx::ResourcePtr<sf::InputStream> Open(const std::filesystem::path& fileName) const override;

        bool Contains(const std::filesystem::path& name) const override;
        std::vector<std::unique_ptr<Gx::FileInfo>> GetFileEntries() const override;
        std::unique_ptr<Gx::FileInfo> GetFileInfo(const std::filesystem::path& fileName) const override;

        std::optional<std::size_t> ReadFile(const std::filesystem::path& fileName, void* data, std::size_t size) const override;
        void WriteFile(const std::filesystem::path& fileName, const void* data, std::size_t size) override { throw Gx::NotSupportedException(); }

        std::optional<std::size_t> GetFileSize(const std::filesystem::path& fileName) const override;

    private:
        enum class ArchiveType
        {
            Unknown,
            M30,
            OMC
        };

        ArchiveType m_type = ArchiveType::Unknown;
    };
}
