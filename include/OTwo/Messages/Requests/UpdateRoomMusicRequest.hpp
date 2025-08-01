#pragma once

#include <OTwo/Models/Game.hpp>
#include <OTwo/Messages/Commands.hpp>

struct UpdateRoomMusicRequest
{
    static constexpr Command Command = RequestCommand::SetWaitingMusic;

    std::uint32_t MusicID{};
    Difficulty    Difficulty{};
    std::uint8_t  SpeedID{};
};
