#ifndef MUSIC_METADATA_HPP
#define MUSIC_METADATA_HPP

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

struct MusicMetadata : public Gx::ResourceMetadata
{
public:
    MusicMetadata() {}
    virtual ~MusicMetadata() {}

    bool Loop;
};

#endif