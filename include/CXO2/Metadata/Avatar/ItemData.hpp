#pragma once

#include <CXO2/Metadata/ResourceMetadata.hpp>
#include <CXO2/Metadata/Avatar/ItemMetadata.hpp>

#include <map>

namespace Cx
{
    struct ItemData : ResourceMetadata
    {
        std::string Version;
        std::map<unsigned int, ItemMetadata> Items;
    };
}
