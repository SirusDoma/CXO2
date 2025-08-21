#pragma once

#include <OTwo/Models/Game.hpp>
#include <OTwo/Messages/Commands.hpp>

struct WaitingMusicChangedEventData
{
    static constexpr ::Command Command = EventCommand::WaitingMusicChanged;

    std::uint32_t MusicID{};
    ::Difficulty  Difficulty{};
    std::uint8_t  SpeedID{};
};
