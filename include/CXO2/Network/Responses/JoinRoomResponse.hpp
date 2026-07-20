#pragma once

#include <CXO2/Models/Room.hpp>
#include <CXO2/Models/Character.hpp>
#include <CXO2/Models/Game.hpp>
#include <CXO2/Network/Commands.hpp>
#include <CXO2/Network/CollectionEnvelope.hpp>
#include <CXO2/Network/StringEnvelope.hpp>

#include <Genode/Network/Packet.hpp>

namespace Cx
{
    enum class JoinResult : std::uint32_t
    {
        Success         = 0x00000000, // 0
        ConnectionError = 0xFFFFFFFF, // -1
        InvalidMode     = 0xFFFFFFFE, // -2
        InvalidPassword = 0xFFFFFFFD, // -3
        InProgress      = 0xFFFFFFFB, // -5
        Full            = 0xFFFFFFFA  // -6 (or -4 / 0xFFFFFFFC)
    };


    struct JoinRoomResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::JoinRoom;

        struct Member
        {
            sf::String  Name{};
            int         Level{};
            Cx::Gender  Gender{};
            bool        IsRoomMaster{};
            Room::Team  Team{};
            bool        Ready{};

            CollectionEnvelope<std::unordered_set<std::uint32_t>> EquippedItemIDs{12, 12};
            CollectionEnvelope<std::unordered_set<std::uint32_t>, std::uint32_t> MusicIDs{};
        };

        struct Slot
        {
            std::uint8_t  Index{};
            Room::SlotState State{};

            JoinRoomResponse::Member Member{};
        };

        JoinResult     Result{};
        std::uint8_t   SlotID{};
        Room::Team     Team{};
        sf::String     Title{};
        std::uint32_t  MusicID{};
        std::uint32_t  Map{};
        GameMode       Mode{};
        Cx::Difficulty Difficulty{};
        Cx::Speed      Speed{};
        std::uint32_t  UserCount{};

        CollectionEnvelope<std::vector<Slot>> Slots{8, 8};
    };

    inline Gx::Packet& operator<<(Gx::Packet& packet, const JoinRoomResponse::Slot& slot)
    {
        packet << slot.Index << slot.State;
        if (slot.State == Room::SlotState::Occupied)
            packet << slot.Member;

        return packet;
    }

    inline Gx::Packet& operator>>(Gx::Packet& packet, JoinRoomResponse::Slot& slot)
    {
        packet >> slot.Index >> slot.State;
        if (slot.State == Room::SlotState::Occupied)
            packet >> slot.Member;

        return packet;
    }
}
