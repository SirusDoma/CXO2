#pragma once

#include <CXO2/Metadata/Chart/ChartMetadata.hpp>
#include <CXO2/Models/Character.hpp>
#include <CXO2/Models/Game.hpp>
#include <CXO2/Models/Map.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/String.hpp>

#include <array>
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
        Unavailable = 0,
        Waiting     = 1,
        Playing     = 2
    };

    enum class RoomTransitionEventType : std::uint8_t
    {
        Normal     = 0,
        Kick       = 1,
        Terminated = 2
    };

    struct Room
    {
        static constexpr std::uint8_t MaxCapacity = 8;

        enum class SlotState : std::uint32_t
        {
            Unoccupied = 0,
            Occupied   = 1,
            Locked     = 2
        };

        enum class SlotEventType : std::uint8_t
        {
            Unlock = 0,
            Lock   = 2,
            Kicked = 3
        };

        enum class Team : std::uint8_t
        {
            A = 0,
            B = 1,
            C = 2,
            D = 3,
            E = 4,
            F = 5,
            G = 6,
            H = 7
        };

        struct Slot
        {
            sf::String   Name;
            Cx::Gender   Gender{};
            std::int32_t Level{};
            EquipmentSet EquippedItemIDs;
            MusicList    MusicIDs;

            SlotState  State{};
            bool       IsMaster{};
            bool       Ready{};
            Room::Team Team{};
            sf::Color  TeamColor{sf::Color::Transparent};
        };

        std::uint32_t  ID{};
        RoomState      State{};
        sf::String     Title;
        bool           Locked{};
        ChartMetadata  Music{};
        Cx::Difficulty Difficulty{};
        GameMode       Mode{};
        float          Speed{1.0f};
        Cx::SpeedMode  SpeedMode{Cx::SpeedMode::HiSpeed};
        LevelCategory  Random{};
        Cx::Map        Map{};
        std::uint8_t   EffectID{1};
        std::uint8_t   MinLevelLimit{};
        std::uint8_t   MaxLevelLimit{};
        std::uint8_t   UserCount{};
        std::uint8_t   Capacity{MaxCapacity};

        std::array<Slot, MaxCapacity> Slots{};
    };
}
