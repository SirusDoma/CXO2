#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Models/Game.hpp>

struct RoomMusicChangedEventData
{
    static constexpr Command Command = EventCommand::RoomMusicChanged;

    std::uint32_t ID{};
    std::uint32_t MusicID{};
    Difficulty    Difficulty{};
    std::uint8_t  SpeedID{};
};
