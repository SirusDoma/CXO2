#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Models/Game.hpp>

namespace Cx
{
    struct RoomMusicChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::RoomMusicChanged;

        std::uint32_t  ID{};
        std::uint32_t  MusicID{};
        Cx::Difficulty Difficulty{};
        Cx::Speed      Speed{};
    };
}
