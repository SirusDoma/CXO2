#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Models/Game.hpp>

namespace Cx
{
    struct RoomCreatedEventData
    {
        static constexpr Cx::Command Command = EventCommand::RoomCreated;

        std::uint32_t ID{};
        sf::String    Title{};
        Cx::GameMode  GameMode{};
        bool          Locked{};
        std::uint8_t  MinLevelLimit{};
        std::uint8_t  MaxLevelLimit{};
    };
}
