#pragma once

#include <CXO2/Messages/StringEnvelope.hpp>
#include <cstdint>

namespace Cx
{
    enum class RoomState  : std::uint8_t;
    enum class GameMode   : std::uint8_t;
    enum class Difficulty : std::uint8_t;

    enum class RoomSlotState : std::uint32_t
    {
        Unoccupied = 0x00000000, // 0
        Occupied   = 0x00000001, // 1
        Locked     = 0x00000002, // 2
    };


    struct RoomInfo
    {
        std::uint32_t    ID{};
        RoomState        State{};
        StringEnvelope<> Title{0, 21};
        bool             Locked{};
        std::uint32_t    MusicID{};
        Cx::Difficulty   Difficulty{};
        GameMode         Mode{};
        std::uint8_t     SpeedID{};
        std::uint8_t     Capacity{};
        std::uint8_t     UserCount{};
        std::uint8_t     MinLevelLimit{};
        std::uint8_t     MaxLevelLimit{};
    };
}
