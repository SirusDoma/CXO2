#pragma once

#include <CXO2/Models/Room.hpp>
#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct UpdateMemberTeamRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SetUserTeam;

        Room::Team Team;
    };
}
