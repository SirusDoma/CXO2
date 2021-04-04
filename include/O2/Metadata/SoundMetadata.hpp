#ifndef SOUND_METADATA_HPP
#define SOUND_METADATA_HPP

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

struct SoundMetadata : public Gx::ResourceMetadata
{
    virtual ~SoundMetadata() {}
};

#endif