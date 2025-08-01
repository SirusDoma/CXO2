#pragma once

#include <OTwo/Messages/Commands.hpp>

struct SubmitScoreRequest
{
    static constexpr Command Command = RequestCommand::SubmitScore;

    std::uint16_t Cool{};
    std::uint16_t Good{};
    std::uint16_t Bad{};
    std::uint16_t Miss{};
    std::uint16_t MaxCombo{};
    std::uint16_t JamCombo{};
    std::uint16_t MaxJamCombo{};
    std::uint32_t Score{};
    std::uint8_t  Life{};
};
