#pragma once

#include <CXO2/Models/Game.hpp>
#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct WaitingMusicChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::WaitingMusicChanged;

        std::uint32_t  MusicID{};
        Cx::Difficulty Difficulty{};
        Cx::Speed      Speed{};
    };
}
