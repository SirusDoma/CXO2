#pragma once

#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <OTwo/Metadata/Avatar/ItemMetadata.hpp>

#include <map>

struct ItemData : ResourceMetadata
{
    std::string Version;
    std::map<unsigned int, ItemMetadata> Items;
};
