#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/ResourceMetadata.hpp>

namespace Cx
{
    class MetadataLoader : public ResourceLoader<ResourceMetadata>
    {
    public:
        [[nodiscard]] Gx::ResourcePtr<ResourceMetadata> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<ResourceMetadata> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;

        [[nodiscard]] static bool Parse(const Gx::Json& json, ResourceMetadata& metadata, const Gx::ResourceContext& ctx = Gx::ResourceContext::Default);
    };
}
