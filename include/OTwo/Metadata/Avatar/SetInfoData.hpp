#pragma once
#include <OTwo/Metadata/Avatar/SetInfoMetadata.hpp>

#include <string>
#include <unordered_map>

struct SetInfoData : ResourceMetadata
{
    std::string Version;
    std::map<unsigned int, SetInfoMetadata> Sets;
};
