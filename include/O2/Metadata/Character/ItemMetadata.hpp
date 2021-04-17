#ifndef O2JAM_CHARACTER_ITEM_METADATA_HPP
#define O2JAM_CHARACTER_ITEM_METADATA_HPP

#include <O2/Data/Character.hpp>
#include <O2/Data/Equipment.hpp>
#include <O2/Data/Planet.hpp>
#include <O2/Data/Shop.hpp>

#include <Genode/IO/ResourceMetadata.hpp>

#include <SFML/System/String.hpp>

struct ItemMetadata : public Gx::ResourceMetadata
{
    ItemMetadata() : References() {};

    struct ItemSpriteMetadata
    {
        Character::Gender     Gender;
        Equipment::RenderPart RenderType;
        Equipment::Instrument Instrument;
        std::string           Reference;
    };

    unsigned int      ID;
    Equipment::Type   EquipmentType;
    Planet::Planet    Origin;
    Character::Gender Gender;
    bool              IsNew;
    sf::String        Name, Description, SmallPreview, LargePreview;
    std::map<Shop::Currency, unsigned int> Prices;
    std::vector<ItemSpriteMetadata> References;
};

#endif
