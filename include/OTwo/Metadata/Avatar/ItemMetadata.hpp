#ifndef O2JAM_AVATAR_ITEM_METADATA_HPP
#define O2JAM_AVATAR_ITEM_METADATA_HPP

#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <OTwo/Data/Shop.hpp>
#include <OTwo/Data/Planet.hpp>
#include <OTwo/Data/Equipment.hpp>
#include <OTwo/Data/Character.hpp>

#include <SFML/System/String.hpp>

struct ItemMetadata : ResourceMetadata
{
    ItemMetadata() : References() {};

    struct ItemSpriteMetadata
    {
        Gender      Gender;
        RenderPart  RenderPart;
        Instrument  Instrument;
        std::string Reference;
    };

    unsigned int                     ID;
    EquipmentType                    EquipmentType;
    PlanetType                       Origin;
    Gender                           Gender;
    bool                             IsNew;
    sf::String                       Name, Description, SmallPreview, LargePreview;
    std::map<Currency, unsigned int> Prices;
    std::vector<ItemSpriteMetadata>  References;
};

#endif
