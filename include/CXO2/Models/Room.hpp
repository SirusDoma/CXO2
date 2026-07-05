#pragma once

#include <cstdint>
#include <cstddef>

namespace Cx
{
    enum class KeyMode : std::size_t
    {
        Three = 3,
        Five  = 5,
        Seven = 7
    };

    enum class RoomState : std::uint8_t
    {
        Unavailable,
        Waiting,
        Playing
    };

    enum class SongMode
    {
        Normal,
        Random
    };

    enum class RoomSlotEventType : std::uint8_t
    {
        Unlock = 0,
        Lock   = 2,
        Kicked = 3
    };

    enum class RoomTransitionEventType : std::uint8_t
    {
        Normal     = 0,
        Kick       = 1,
        Terminated = 2
    };

    enum class RoomTeam : std::uint8_t
    {
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H
    };
}
