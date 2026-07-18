#pragma once

#include <CXO2/Network/StringEnvelope.hpp>
#include <cstdint>

namespace Cx
{
    enum class RoomState  : std::uint8_t;
    enum class GameMode   : std::uint8_t;
    enum class Difficulty : std::uint8_t;
    enum class Speed      : std::uint8_t;

    enum class RoomSlotState : std::uint32_t
    {
        Unoccupied = 0x00000000, // 0
        Occupied   = 0x00000001, // 1
        Locked     = 0x00000002, // 2
    };


    struct RoomInfo
    {
        std::uint32_t  ID{};
        RoomState      State{};
        sf::String     Title;
        bool           Locked{};
        std::uint32_t  MusicID{};
        Cx::Difficulty Difficulty{};
        GameMode       Mode{};
        Cx::Speed      Speed{};
        std::uint8_t   Capacity{};
        std::uint8_t   UserCount{};
        std::uint8_t   MinLevelLimit{};
        std::uint8_t   MaxLevelLimit{};
    };
}
