#pragma once

#include <CXO2/Models/Room.hpp>
#include <CXO2/Network/Commands.hpp>
#include <CXO2/Network/CollectionEnvelope.hpp>
#include <CXO2/Network/StringEnvelope.hpp>

#include <cstdint>

namespace Cx
{
    struct RoomListResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::GetRoomList;

        struct Room
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

            operator Cx::Room() const;
        };

        CollectionEnvelope<std::vector<Room>, std::uint32_t> Rooms{0, 100};
    };

    inline RoomListResponse::Room::operator Cx::Room() const
    {
        const auto music = MusicSelection{MusicID};

        auto room = Cx::Room{};
        room.ID            = ID;
        room.State         = State;
        room.Title         = Title;
        room.Locked        = Locked;
        room.Music         = ChartMetadata{music.ID};
        room.Random        = music.Random;
        room.Difficulty    = Difficulty;
        room.Mode          = Mode;
        room.Speed         = ToSpeedValue(Speed).value_or(1.0f);
        room.SpeedMode     = ToSpeedMode(Speed);
        room.Capacity      = Capacity;
        room.UserCount     = UserCount;
        room.MinLevelLimit = MinLevelLimit;
        room.MaxLevelLimit = MaxLevelLimit;

        return room;
    }
}
