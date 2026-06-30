#pragma once

#include <CXO2/Metadata/ResourceMetadata.hpp>
#include <CXO2/Models/Character.hpp>
#include <CXO2/Models/Planet.hpp>
#include <CXO2/Models/Equipment.hpp>
#include <CXO2/Models/Shop.hpp>

#include <SFML/System/String.hpp>

namespace Cx
{
    struct ItemMetadata : ResourceMetadata
    {
        struct ItemSpriteMetadata
        {
            Cx::Gender      Gender;
            Cx::RenderPart  RenderPart;
            Cx::Instrument  Instrument;
            Gx::Json      Reference;
        };

        unsigned int                     ID;
        Cx::EquipmentType                  EquipmentType;
        Cx::Planet                         Origin;
        Cx::Gender                         Gender;
        bool                             IsNew;
        sf::String                       Name, Description;
        Gx::Json                         SmallThumbnail, LargeThumbnail;
        std::map<Currency, unsigned int> Prices;
        std::vector<ItemSpriteMetadata>  References = std::vector<ItemSpriteMetadata>();
    };
}
