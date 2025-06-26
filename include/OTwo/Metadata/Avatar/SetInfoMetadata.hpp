#pragma once

#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <OTwo/Models/Character.hpp>
#include <OTwo/Models/Planet.hpp>
#include <OTwo/Models/Shop.hpp>

#include <SFML/System/String.hpp>

#include <map>
#include <unordered_set>

using DiscountMap = std::map<std::uint32_t, std::unordered_map<Currency, std::uint32_t>>;
struct SetInfoMetadata : ResourceMetadata
{
    unsigned int                     ID;
    sf::String                       Name, Description;
    ::Planet                         Origin;
    ::Gender                         Gender;
    bool                             IsNew;
    DiscountMap                      Discounts = {};
    std::unordered_set<unsigned int> ItemsIDs  = {};
};
