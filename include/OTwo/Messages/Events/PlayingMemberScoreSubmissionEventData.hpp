#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Models/Room.hpp>

struct PlayingMemberScoreSubmissionEventData
{
    static constexpr Command Command = EventCommand::ScoreSubmission;

    std::uint8_t    ID{};
};
