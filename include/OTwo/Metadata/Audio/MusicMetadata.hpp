#ifndef O2JAM_MUSIC_METADATA_HPP
#define O2JAM_MUSIC_METADATA_HPP

#include <OTwo/Metadata/ResourceMetadata.hpp>

struct MusicMetadata : public ResourceMetadata
{
    std::string Source;
    bool IsLoop;
};

#endif