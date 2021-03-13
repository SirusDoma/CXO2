#ifndef SPRITE_METADATA_LOADER_HPP
#define SPRITE_METADATA_LOADER_HPP

#include <Genode/IO/Json.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <O2/IO/Metadata/SpriteMetadata.hpp>

#include <memory>

class SpriteMetadataLoader : public Gx::MetadataLoader<Gx::Sprite>
{
public :
    SpriteMetadataLoader();

    virtual Gx::ResourceMetadata* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual Gx::Sprite* Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const;
};

#endif