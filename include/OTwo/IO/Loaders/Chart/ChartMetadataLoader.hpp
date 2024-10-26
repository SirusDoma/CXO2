#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/Chart/ChartMetadata.hpp>

class ChartMetadataLoader : public Gx::ResourceLoader<ChartMetadata>
{
public:
    Gx::ResourcePtr<ChartMetadata> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<ChartMetadata> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<ChartMetadata> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;
};
