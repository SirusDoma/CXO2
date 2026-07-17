#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Models/Room.hpp>

namespace Cx
{
    struct PlayingMemberScoreSubmissionEventData
    {
        static constexpr Cx::Command Command = EventCommand::ScoreSubmission;

        std::uint8_t    ID{};
    };
}
