#ifndef O2JAM_CHART_LOADER_HPP
#define O2JAM_CHART_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Chart/Chart.hpp>

class O2ChartLoader : public Gx::ResourceLoader<O2Chart>
{
public:
    O2ChartLoader() = default;

    Gx::ResourcePtr<O2Chart> LoadFromMetadata(const O2ChartMetadata &meta, const Gx::ResourceContext &ctx) const;
    Gx::ResourcePtr<O2Chart> LoadFromFile(const std::string &fileName, const Gx::ResourceContext &ctx) const override;
    Gx::ResourcePtr<O2Chart> LoadFromMemory(void *data, std::size_t size, const Gx::ResourceContext &ctx) const override;
    Gx::ResourcePtr<O2Chart> LoadFromStream(sf::InputStream &stream, const Gx::ResourceContext &ctx) const override;

    static Gx::ResourcePtr<sf::Texture> LoadThumbnail(O2ChartMetadata &metadata, const Gx::ResourceContext &ctx);
    static Gx::ResourcePtr<sf::Texture> LoadCoverArt(O2ChartMetadata &metadata, const Gx::ResourceContext &ctx);
    static Gx::ResourcePtr<sf::Texture> LoadThumbnail(sf::InputStream &stream, O2ChartMetadata &metadata, const Gx::ResourceContext &ctx);
    static Gx::ResourcePtr<sf::Texture> LoadCoverArt(sf::InputStream &stream, O2ChartMetadata &metadata, const Gx::ResourceContext &ctx);
};

#endif
