#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/UI/ImageMetadata.hpp>

namespace Cx
{
    class ImageLoader : public ResourceLoader<Gx::Image>
    {
    public :
        ImageLoader() = default;

        static void OnRegistered(const std::string& id, const Builder& builder);

        Gx::ResourcePtr<Gx::Image> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<Gx::Image> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

        static bool ParseMetadata(const Gx::Json& attributes, ImageMetadata& metadata, const Gx::ResourceContext& context);
    };
}
