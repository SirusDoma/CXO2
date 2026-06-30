#pragma once

#include <cstdint>

namespace Cx
{
    struct ScoreEntry
    {
        std::uint8_t  ID{};
        std::int32_t  Active{};
        std::uint16_t Cool{};
        std::uint16_t Good{};
        std::uint16_t Bad{};
        std::uint16_t Miss{};
        std::uint16_t MaxCombo{};
        std::uint16_t MaxJamCombo{};
        std::uint32_t Score{};
        std::uint16_t Reward{};
        std::int32_t  Level{};
        std::int32_t  Experience{};
        bool          IsWinning{};
        std::uint8_t  Reserved{};
    };
}
