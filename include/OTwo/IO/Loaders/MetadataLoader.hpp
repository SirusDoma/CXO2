#ifndef O2JAM_METADATA_LOADER_HPP
#define O2JAM_METADATA_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/ResourceMetadata.hpp>

class MetadataLoader : public ResourceLoader<ResourceMetadata>
{
public:
    Gx::ResourcePtr<ResourceMetadata> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const override;
    Gx::ResourcePtr<ResourceMetadata> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;

    static bool Parse(const Gx::Json &json, ResourceMetadata &metadata, const Gx::ResourceContext &ctx = Gx::ResourceContext::Default);
};

#endif
