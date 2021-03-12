#ifndef MUSIC_DEFINITION_LOADER
#define MUSIC_DEFINITION_LOADER

#include <SFML/Audio/Music.hpp>

#include <Genode/IO/Json.hpp>
#include <Genode/IO/DefinitionLoader.hpp>

#include <Genode/IO/ResourceDefinition.hpp>
#include <Genode/IO/ResourceContext.hpp>

#include <O2/IO/Definitions/MusicDefinition.hpp>

#include <memory>

class MusicDefinitionLoader : public Gx::DefinitionLoader<sf::Music>
{
public :
    MusicDefinitionLoader();

    virtual Gx::ResourceDefinition* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual sf::Music* Create(Gx::ResourceDefinition* definition, Gx::ResourceContext context) const;
};

#endif