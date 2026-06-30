#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <CXO2/Models/Room.hpp>

namespace Cx
{
    enum class UpdateStatsType : std::uint16_t;
    struct UpdateGameStatsRequest
    {
        static constexpr Cx::Command Command = RequestCommand::UpdateGameStats;

        UpdateStatsType Type{};
        std::uint16_t   Value{};
    };
}
