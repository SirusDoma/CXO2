#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Models/Room.hpp>

struct WaitingMemberTeamChangedEventData
{
    static constexpr Command Command = EventCommand::UserTeamChanged;

    std::uint8_t ID{};
    RoomTeam     Team{};
};
