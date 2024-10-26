#pragma once

#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <OTwo/Serializable.g.hpp>
#include <OTwo/Models/Equipment.hpp>

#include <SFML/System/String.hpp>

struct ItemMetadata : ResourceMetadata
{
    struct ItemSpriteMetadata
    {
        ::Gender      Gender;
        ::RenderPart  RenderPart;
        ::Instrument  Instrument;
        std::string   Reference;
    };

    unsigned int                     ID;
    ::EquipmentType                  EquipmentType;
    ::Planet                         Origin;
    ::Gender                         Gender;
    bool                             IsNew;
    sf::String                       Name, Description, SmallThumbnail, LargeThumbnail;
    std::map<Currency, unsigned int> Prices;
    std::vector<ItemSpriteMetadata>  References = std::vector<ItemSpriteMetadata>();
};
