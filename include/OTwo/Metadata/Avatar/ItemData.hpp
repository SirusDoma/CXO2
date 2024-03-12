#ifndef O2JAM_AVATAR_ITEM_DATA_HPP
#define O2JAM_AVATAR_ITEM_DATA_HPP

#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <OTwo/Metadata/Avatar/ItemMetadata.hpp>

#include <map>

struct ItemData : ResourceMetadata
{
    std::string Version;
    std::map<unsigned int, ItemMetadata> Items;
};

#endif
