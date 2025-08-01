#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Core/Chart.hpp>

#include <SFML/Graphics/Image.hpp>

class GameContext;
class O2JamChartLoader : public Gx::ResourceLoader<Chart>
{
public:
    explicit O2JamChartLoader(const GameContext& context);

    Gx::ResourcePtr<Chart> LoadFromMetadata(const O2JamChartMetadata& meta, const Gx::ResourceContext& ctx) const;
    Gx::ResourcePtr<Chart> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<Chart> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<Chart> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;

    void SetCoverLoadCallback(const std::function<void(const sf::Image *)> &onCoverLoaded);
    void SetThumbnailLoadCallback(const std::function<void(const sf::Image *)> &onThumbnailLoaded);


    static Gx::ResourcePtr<sf::Image> LoadThumbnail(const std::string& source, const Gx::ResourceContext& ctx);
    static Gx::ResourcePtr<sf::Image> LoadCoverArt(const std::string& source, const Gx::ResourceContext& ctx);
    static Gx::ResourcePtr<sf::Image> LoadThumbnail(const O2JamChartMetadata& metadata, const Gx::ResourceContext& ctx);
    static Gx::ResourcePtr<sf::Image> LoadCoverArt(const O2JamChartMetadata& metadata, const Gx::ResourceContext& ctx);
    static Gx::ResourcePtr<sf::Image> LoadThumbnail(sf::InputStream& stream, const O2JamChartMetadata& metadata, const Gx::ResourceContext& ctx);
    static Gx::ResourcePtr<sf::Image> LoadCoverArt(sf::InputStream& stream, const O2JamChartMetadata& metadata, const Gx::ResourceContext& ctx);

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
