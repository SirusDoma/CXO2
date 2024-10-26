#pragma once

#include <OTwo/Metadata/ResourceMetadata.hpp>

struct MusicMetadata : public ResourceMetadata
{
    std::string Source;
    bool IsLoop;
};
