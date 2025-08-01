#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/Chart/O2JamChartMetadata.hpp>

class O2JamChartMetadataLoader : public Gx::ResourceLoader<O2JamChartMetadata>
{
public:
    Gx::ResourcePtr<O2JamChartMetadata> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<O2JamChartMetadata> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<O2JamChartMetadata> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;
};
