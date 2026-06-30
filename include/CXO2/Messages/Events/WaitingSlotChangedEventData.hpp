#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    enum class RoomSlotEventType : std::uint8_t;

    struct WaitingSlotChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::RoomSlotChanged;

        std::uint8_t      ID{};
        RoomSlotEventType Type{};
    };
}
