#ifndef SPRITE_DEFINITION_LOADER
#define SPRITE_DEFINITION_LOADER

#include <Genode/IO/DefinitionLoader.hpp>
#include <Genode/IO/ResourceDefinition.hpp>
#include <Genode/IO/Json.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <Nx/IO/Definitions/SpriteDefinition.hpp>

#include <memory>

class SpriteDefinitionLoader : public Gx::DefinitionLoader<Gx::Sprite>
{
public :
    SpriteDefinitionLoader();

    virtual Gx::ResourceDefinition* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual Gx::Sprite* Create(Gx::ResourceDefinition* definition, Gx::ResourceContext context) const;
};

#endif