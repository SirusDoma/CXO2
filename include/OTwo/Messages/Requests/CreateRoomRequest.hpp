#pragma once

#include <OTwo/Models/Game.hpp>
#include <OTwo/Messages/Commands.hpp>

struct CreateRoomRequest
{
    static constexpr ::Command Command = RequestCommand::CreateRoom;

    std::string  Title{};
    ::GameMode   GameMode{};
    bool         Locked{};
    std::string  Password{};
    std::uint8_t MinLevelLimit{};
    std::uint8_t MaxLevelLimit{};
};
