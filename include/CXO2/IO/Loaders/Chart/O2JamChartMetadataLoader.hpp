#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/Chart/O2JamChartMetadata.hpp>

namespace Cx
{
    class O2JamChartMetadataLoader : public Gx::ResourceLoader<O2JamChartMetadata>
    {
    public:
        Gx::ResourcePtr<O2JamChartMetadata> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;
        Gx::ResourcePtr<O2JamChartMetadata> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
        Gx::ResourcePtr<O2JamChartMetadata> LoadFromStream(sf::InputStream& input, const Gx::ResourceContext& ctx) const override;
    };
}
