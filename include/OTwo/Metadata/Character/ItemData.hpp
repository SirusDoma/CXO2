#ifndef O2JAM_CHARACTER_ITEM_DATA_HPP
#define O2JAM_CHARACTER_ITEM_DATA_HPP

#include <OTwo/Metadata/Character/ItemMetadata.hpp>
#include <map>

struct ItemData : Gx::ResourceMetadata
{
    std::string Version;
    std::map<unsigned int, ItemMetadata> Items;
};

#endif
