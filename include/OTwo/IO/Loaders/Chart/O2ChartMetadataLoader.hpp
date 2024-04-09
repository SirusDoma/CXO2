#ifndef O2JAM_CHART_METADATA_LOADER_HPP
#define O2JAM_CHART_METADATA_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/Chart/O2ChartMetadata.hpp>

class O2ChartMetadataLoader : public Gx::ResourceLoader<O2ChartMetadata>
{
public:
    Gx::ResourcePtr<O2ChartMetadata> LoadFromFile(const std::string &fileName, const Gx::ResourceContext &ctx) const override;
    Gx::ResourcePtr<O2ChartMetadata> LoadFromMemory(void *data, std::size_t size, const Gx::ResourceContext &ctx) const override;
    Gx::ResourcePtr<O2ChartMetadata> LoadFromStream(sf::InputStream &stream, const Gx::ResourceContext &ctx) const override;
};

#endif
