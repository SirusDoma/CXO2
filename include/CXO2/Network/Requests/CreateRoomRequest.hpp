#pragma once

#include <CXO2/Models/Game.hpp>
#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct CreateRoomRequest
    {
        static constexpr Cx::Command Command = RequestCommand::CreateRoom;

        sf::String   Title{};
        Cx::GameMode GameMode{};
        bool         Locked{};
        std::string  Password{};
        std::uint8_t MinLevelLimit{};
        std::uint8_t MaxLevelLimit{};
    };
}
