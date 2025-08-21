#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Models/Room.hpp>

struct WaitingMemberLeftEventData
{
    static constexpr ::Command Command = EventCommand::UserLeaveWaiting;

    std::uint8_t ID{};
    std::uint8_t RoomMasterID{};
};
