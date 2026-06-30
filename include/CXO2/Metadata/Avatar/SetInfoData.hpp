#pragma once
#include <CXO2/Metadata/Avatar/SetInfoMetadata.hpp>

#include <string>
#include <unordered_map>

namespace Cx
{
    struct SetInfoData : ResourceMetadata
    {
        std::string Version;
        std::map<unsigned int, SetInfoMetadata> Sets;
    };
}
