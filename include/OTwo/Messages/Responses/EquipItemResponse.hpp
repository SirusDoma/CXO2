#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Messages/Requests/EquipItemRequest.hpp>

struct EquipItemResponse
{
    static constexpr ::Command Command = ResponseCommand::EquipItem;

    std::uint32_t     Invalid{};
    ItemEquipSlotType EquipSlot{};
    std::uint32_t     NewEquippedItemId{};
    std::uint32_t     SlotID{};
    std::uint32_t     PreviousEquippedItemId{};
};
