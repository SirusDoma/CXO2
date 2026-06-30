#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <CXO2/Models/Room.hpp>

namespace Cx
{
    enum class UpdateStatsType : std::uint16_t
    {
        Life = 0x0000,
        Jam  = 0x0001
    };

    struct PlayingMemberStatsUpdateEventData
    {
        static constexpr Cx::Command Command = EventCommand::GameStatsUpdate;

        std::uint8_t    ID{};
        UpdateStatsType Type{};
        std::uint16_t   Value{};
    };
}
