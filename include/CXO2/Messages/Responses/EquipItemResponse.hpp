#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <CXO2/Messages/Requests/EquipItemRequest.hpp>

namespace Cx
{
    struct EquipItemResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::EquipItem;

        std::uint32_t     Invalid{};
        ItemEquipSlotType EquipSlot{};
        std::uint32_t     NewEquippedItemId{};
        std::uint32_t     SlotID{};
        std::uint32_t     PreviousEquippedItemId{};
    };
}
