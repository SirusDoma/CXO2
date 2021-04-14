#ifndef O2JAM_MUSIC_METADATA_HPP
#define O2JAM_MUSIC_METADATA_HPP

#include <Genode/IO/ResourceMetadata.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

struct MusicMetadata : public Gx::ResourceMetadata
{
    std::string Source;
    bool IsLoop;
};

#endif