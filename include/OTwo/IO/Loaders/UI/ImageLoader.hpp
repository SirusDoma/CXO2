#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/UI/ImageMetadata.hpp>

class ImageLoader : public ResourceLoader<Gx::Image>
{
public :
    ImageLoader() = default;

    Gx::ResourcePtr<Gx::Image> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::Image> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

    static bool ParseMetadata(const Gx::Json& attributes, ImageMetadata& metadata, const Gx::ResourceContext& context);
};
