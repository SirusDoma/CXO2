#ifndef SOUND_METADATA_LOADER_HPP
#define SOUND_METADATA_LOADER_HPP

#include <SFML/Audio/Sound.hpp>

#include <Genode/IO/Json.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>

#include <O2/IO/Metadata/SoundMetadata.hpp>

class SoundLoader : public Gx::MetadataLoader<sf::Sound>
{
public :
    SoundLoader();

    virtual Gx::ResourceMetadata* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual sf::Sound* Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const;
};

#endif