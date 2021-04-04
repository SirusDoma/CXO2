#ifndef O2JAM_SPRITE_LOADER_HPP
#define O2JAM_SPRITE_LOADER_HPP

#include <Genode/Graphics/Sprite.hpp>
#include <O2/Loaders/O2JamResourceLoader.hpp>

class SpriteMetadata;
class SpriteLoader : public O2JamResourceLoader<Gx::Sprite>
{
public :
    SpriteLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::Sprite> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;

    static void ParseSprite(Json attributes, SpriteMetadata &metadata);
};

#endif