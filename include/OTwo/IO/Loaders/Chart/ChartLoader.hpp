#ifndef O2JAM_CHART_LOADER_HPP
#define O2JAM_CHART_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Core/Chart.hpp>

#include <SFML/Graphics/Image.hpp>

class ChartLoader : public Gx::ResourceLoader<Chart>
{
public:
    ChartLoader() = default;

    Gx::ResourcePtr<Chart> LoadFromMetadata(const ChartMetadata &meta, const Gx::ResourceContext& ctx) const;
    Gx::ResourcePtr<Chart> LoadFromFile(const std::string &fileName, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<Chart> LoadFromMemory(void *data, std::size_t size, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<Chart> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;

    void SetCoverLoadCallback(const std::function<void(const sf::Image *)> &onCoverLoaded);
    void SetThumbnailLoadCallback(const std::function<void(const sf::Image *)> &onThumbnailLoaded);

    static Gx::ResourcePtr<sf::Image> LoadThumbnail(const ChartMetadata& metadata, const Gx::ResourceContext& ctx);
    static Gx::ResourcePtr<sf::Image> LoadCoverArt(const ChartMetadata& metadata, const Gx::ResourceContext& ctx);
    static Gx::ResourcePtr<sf::Image> LoadThumbnail(sf::InputStream& stream, const ChartMetadata& metadata, const Gx::ResourceContext& ctx);
    static Gx::ResourcePtr<sf::Image> LoadCoverArt(sf::InputStream& stream, const ChartMetadata& metadata, const Gx::ResourceContext& ctx);

private:
    struct NoteBlockHeader
    {
        Gx::Uint32 Measure;
        Gx::Uint16 Channel;
        Gx::Uint16 EventCount;
    };

    struct NoteEventHeader
    {
        Gx::Uint16 ID;
        Gx::Int8   Audio;
        Gx::Int8   Type;
    };

    std::function<void(const sf::Image*)> m_onCoverLoaded;
    std::function<void(const sf::Image*)> m_onThumbnailLoaded;
};

#endif
