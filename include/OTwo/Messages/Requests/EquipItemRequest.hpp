#pragma once

#include <OTwo/Messages/Commands.hpp>

enum class ItemEquipSlotType : std::uint32_t
{
    Instrument         = 0,
    Hair               = 1,
    Earrings           = 2,
    Gloves             = 3,
    Accessories        = 4,
    Top                = 5,
    Pants              = 6,
    Glasses            = 7,
    Necklace           = 8,
    ClothesAccessories = 9,
    Shoes              = 10,
    Face               = 11,
};


struct EquipItemRequest
{
    static constexpr Command Command = RequestCommand::EquipItem;

    ItemEquipSlotType EquipSlot{};
    std::uint32_t     SlotID{};
};