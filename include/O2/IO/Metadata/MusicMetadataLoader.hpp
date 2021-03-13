#ifndef MUSIC_METADATA_LOADER_HPP
#define MUSIC_METADATA_LOADER_HPP

#include <SFML/Audio/Music.hpp>

#include <Genode/IO/Json.hpp>
#include <Genode/IO/MetadataLoader.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>

#include <O2/IO/Metadata/MusicMetadata.hpp>

#include <memory>

class MusicMetadataLoader : public Gx::MetadataLoader<sf::Music>
{
public :
    MusicMetadataLoader();

    virtual Gx::ResourceMetadata* Load(Gx::Uint8* data, Gx::Uint64 size) const;
    virtual sf::Music* Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const;
};

#endif