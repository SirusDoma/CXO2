#pragma once

#include <CXO2/Metadata/ResourceMetadata.hpp>

namespace Cx
{
    struct MusicMetadata : public ResourceMetadata
    {
        std::string Source;
        bool IsLoop;
    };
}
