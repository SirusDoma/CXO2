#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Models/Room.hpp>

struct PlayingMemberLeftEventData
{
    static constexpr Command Command = EventCommand::UserLeaveGame;

    std::uint8_t ID{};
    std::int32_t Level{};
};
