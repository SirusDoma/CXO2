#pragma once

#include <OTwo/Messages/Commands.hpp>

enum class RoomSlotEventType : std::uint8_t;

struct WaitingSlotChangedEventData
{
    static constexpr ::Command Command = EventCommand::RoomSlotChanged;

    std::uint8_t      ID{};
    RoomSlotEventType Type{};
};
