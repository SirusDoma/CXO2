#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Models/Room.hpp>

namespace Cx
{
    struct PlayingMemberLeftEventData
    {
        static constexpr Cx::Command Command = EventCommand::UserLeaveGame;

        std::uint8_t ID{};
        std::int32_t Level{};
    };
}
