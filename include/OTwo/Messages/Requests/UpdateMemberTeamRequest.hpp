#pragma once

#include <OTwo/Models/Room.hpp>
#include <OTwo/Messages/Commands.hpp>

struct UpdateMemberTeamRequest
{
    static constexpr ::Command Command = RequestCommand::SetUserTeam;

    RoomTeam Team;
};
