#pragma once

#include <vector>
#include <cstdint>

enum class Currency : std::uint8_t
{
    None,
    Gem,
    Cash
};

enum class CartItemType
{
    Equipment,
    EquipmentSet,
    Music
};

struct CartItem
{
    unsigned int ID;
    CartItemType Type;
};

struct Cart
{
    std::vector<CartItem> Items = {};
};