#pragma once

#include <CXO2/Models/Room.hpp>
#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct WaitingSlotChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::RoomSlotChanged;

        std::uint8_t      ID{};
        Room::SlotEventType Type{};
    };
}
