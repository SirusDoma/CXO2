#ifndef O2JAM_SPRITE_LOADER_HPP
#define O2JAM_SPRITE_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <Genode/Graphics/Sprite.hpp>

struct SpriteMetadata;
class SpriteLoader : public ResourceLoader<Gx::Sprite>
{
public :
    SpriteLoader() = default;

    Gx::ResourcePtr<Gx::Sprite> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<Gx::Sprite> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& ctx) const override;

    static bool ParseMetadata(Gx::Json json, SpriteMetadata& metadata, const Gx::ResourceContext& ctx = Gx::ResourceContext::Default);
};

#endif