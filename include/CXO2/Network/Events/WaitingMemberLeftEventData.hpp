#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Models/Room.hpp>

namespace Cx
{
    struct WaitingMemberLeftEventData
    {
        static constexpr Cx::Command Command = EventCommand::UserLeaveWaiting;

        std::uint8_t ID{};
        std::uint8_t RoomMasterID{};
    };
}
