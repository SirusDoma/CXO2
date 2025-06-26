#pragma once

#include <vector>

enum class Currency
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