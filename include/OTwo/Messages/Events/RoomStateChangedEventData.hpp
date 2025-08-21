#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Models/Room.hpp>

struct RoomStateChangedEventData
{
    static constexpr ::Command Command = EventCommand::RoomStateChanged;

    std::uint32_t ID{};
    RoomState     State{};
};
