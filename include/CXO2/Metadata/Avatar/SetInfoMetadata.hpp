#pragma once

#include <CXO2/Metadata/ResourceMetadata.hpp>
#include <CXO2/Models/Character.hpp>
#include <CXO2/Models/Planet.hpp>
#include <CXO2/Models/Shop.hpp>

#include <SFML/System/String.hpp>

#include <map>
#include <unordered_set>

namespace Cx
{
    using DiscountMap = std::map<std::uint32_t, std::unordered_map<Currency, std::uint32_t>>;
    struct SetInfoMetadata : ResourceMetadata
    {
        unsigned int                     ID;
        sf::String                       Name, Description;
        Cx::Planet                         Origin;
        Cx::Gender                         Gender;
        bool                             IsNew;
        DiscountMap                      Discounts = {};
        std::unordered_set<unsigned int> ItemsIDs  = {};
    };
}
