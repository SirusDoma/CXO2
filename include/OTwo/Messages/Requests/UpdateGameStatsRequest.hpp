#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Models/Room.hpp>

enum class UpdateStatsType : std::uint16_t;
struct UpdateGameStatsRequest
{
    static constexpr Command Command = RequestCommand::UpdateGameStats;

    UpdateStatsType Type{};
    std::uint16_t   Value{};
};
