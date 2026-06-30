#pragma once

#include <CXO2/Models/Game.hpp>
#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct UpdateRoomMusicRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SetWaitingMusic;

        std::uint32_t MusicID{};
        Cx::Difficulty  Difficulty{};
        std::uint8_t  SpeedID{};
    };
}
