#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <Genode/Graphics/Sprite.hpp>

namespace Cx
{
    struct SpriteMetadata;
    class SpriteLoader : public ResourceLoader<Gx::Sprite>
    {
    public:
        SpriteLoader() = default;

        Gx::ResourcePtr<Gx::Sprite> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<Gx::Sprite> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

        static bool ParseMetadata(const Gx::Json& attributes, SpriteMetadata& metadata, const Gx::ResourceContext& ctx = Gx::ResourceContext::Default);
    };
}
