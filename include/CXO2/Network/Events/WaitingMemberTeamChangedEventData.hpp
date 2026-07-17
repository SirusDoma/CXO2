#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Models/Room.hpp>

namespace Cx
{
    struct WaitingMemberTeamChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::UserTeamChanged;

        std::uint8_t ID{};
        RoomTeam     Team{};
    };
}
