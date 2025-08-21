#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Models/Room.hpp>

enum class UpdateStatsType : std::uint16_t
{
    Life = 0x0000,
    Jam  = 0x0001
};

struct PlayingMemberStatsUpdateEventData
{
    static constexpr ::Command Command = EventCommand::GameStatsUpdate;

    std::uint8_t    ID{};
    UpdateStatsType Type{};
    std::uint16_t   Value{};
};
