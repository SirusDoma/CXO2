#pragma once

#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <OTwo/Models/Character.hpp>
#include <OTwo/Models/Planet.hpp>
#include <OTwo/Models/Equipment.hpp>
#include <OTwo/Models/Shop.hpp>

#include <SFML/System/String.hpp>

struct ItemMetadata : ResourceMetadata
{
    struct ItemSpriteMetadata
    {
        ::Gender      Gender;
        ::RenderPart  RenderPart;
        ::Instrument  Instrument;
        Gx::Json      Reference;
    };

    unsigned int                     ID;
    ::EquipmentType                  EquipmentType;
    ::Planet                         Origin;
    ::Gender                         Gender;
    bool                             IsNew;
    sf::String                       Name, Description;
    Gx::Json                         SmallThumbnail, LargeThumbnail;
    std::map<Currency, unsigned int> Prices;
    std::vector<ItemSpriteMetadata>  References = std::vector<ItemSpriteMetadata>();
};
