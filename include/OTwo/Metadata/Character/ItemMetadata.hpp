#ifndef O2JAM_CHARACTER_ITEM_METADATA_HPP
#define O2JAM_CHARACTER_ITEM_METADATA_HPP

#include <OTwo/Data/Character.hpp>
#include <OTwo/Data/Equipment.hpp>
#include <OTwo/Data/Planet.hpp>
#include <OTwo/Data/Shop.hpp>

#include <Genode/IO/ResourceMetadata.hpp>

#include <SFML/System/String.hpp>

struct ItemMetadata : public Gx::ResourceMetadata
{
    ItemMetadata() : References() {};

    struct ItemSpriteMetadata
    {
        Character::Gender     Gender;
        Equipment::RenderPart RenderPart;
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
