#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Models/Game.hpp>

struct RoomCreatedEventData
{
    static constexpr Command Command = EventCommand::RoomCreated;

    std::uint32_t ID{};
    std::string   Title{};
    GameMode      GameMode{};
    bool          Locked{};
    std::uint8_t  MinLevelLimit{};
    std::uint8_t  MaxLevelLimit{};
};
