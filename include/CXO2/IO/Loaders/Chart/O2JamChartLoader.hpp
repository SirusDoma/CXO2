#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Core/Chart.hpp>

#include <SFML/Graphics/Image.hpp>

namespace Cx
{
    class O2JamChartLoader : public Gx::ResourceLoader<Chart>
    {
    public:
        O2JamChartLoader(GameMode mode, Difficulty difficulty);

        [[nodiscard]] Gx::ResourcePtr<Chart> LoadFromMetadata(const O2JamChartMetadata& meta, const Gx::ResourceContext& ctx) const;
        [[nodiscard]] Gx::ResourcePtr<Chart> LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<Chart> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<Chart> LoadFromStream(sf::InputStream& input, const Gx::ResourceContext& ctx) const override;

        void SetCoverLoadCallback(const std::function<void(const sf::Image *)> &onCoverLoaded);
        void SetThumbnailLoadCallback(const std::function<void(const sf::Image *)> &onThumbnailLoaded);


        [[nodiscard]] static Gx::ResourcePtr<sf::Image> LoadThumbnail(const std::string& source, const Gx::ResourceContext& ctx);
        [[nodiscard]] static Gx::ResourcePtr<sf::Image> LoadCoverArt(const std::string& source, const Gx::ResourceContext& ctx);
        [[nodiscard]] static Gx::ResourcePtr<sf::Image> LoadThumbnail(const O2JamChartMetadata& metadata, const Gx::ResourceContext& ctx);
        [[nodiscard]] static Gx::ResourcePtr<sf::Image> LoadCoverArt(const O2JamChartMetadata& metadata, const Gx::ResourceContext& ctx);
        [[nodiscard]] static Gx::ResourcePtr<sf::Image> LoadThumbnail(sf::InputStream& input, const O2JamChartMetadata& metadata, const Gx::ResourceContext& ctx);
        [[nodiscard]] static Gx::ResourcePtr<sf::Image> LoadCoverArt(sf::InputStream& input, const O2JamChartMetadata& metadata, const Gx::ResourceContext& ctx);
        [[nodiscard]] static Gx::ResourcePtr<sf::InputStream> Decrypt(Gx::ResourcePtr<sf::InputStream> stream);

    private:
        struct NoteBlockHeader
        {
            std::uint32_t Measure;
            std::uint16_t Channel;
            std::uint16_t EventCount;
        };

        struct NoteEventHeader
        {
            std::uint16_t ID;
            std::int8_t   Audio;
            std::int8_t   Type;
        };

        std::function<void(const sf::Image*)> m_onCoverLoaded;
        std::function<void(const sf::Image*)> m_onThumbnailLoaded;
        GameMode m_mode;
        Difficulty m_difficulty;
    };
}
