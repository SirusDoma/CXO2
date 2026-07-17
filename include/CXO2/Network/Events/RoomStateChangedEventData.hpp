#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Models/Room.hpp>

namespace Cx
{
    struct RoomStateChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::RoomStateChanged;

        std::uint32_t ID{};
        RoomState     State{};
    };
}
