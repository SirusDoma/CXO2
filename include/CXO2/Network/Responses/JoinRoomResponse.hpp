#pragma once

#include <CXO2/Models/Room.hpp>
#include <CXO2/Models/Character.hpp>
#include <CXO2/Network/Commands.hpp>
#include <CXO2/Network/MapInfo.hpp>
#include <CXO2/Network/RoomInfo.hpp>

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

        struct MemberInfo
        {
            std::string Name{};
            int         Level{};
            Cx::Gender  Gender{};
            bool        IsRoomMaster{};
            RoomTeam    Team{};
            bool        Ready{};

            CollectionEnvelope<std::unordered_set<std::uint32_t>> EquippedItemIDs{12, 12};
            CollectionEnvelope<std::unordered_set<std::uint32_t>, std::uint32_t> MusicIDs{};
        };

        struct SlotInfo
        {
            std::uint8_t  Index{};
            RoomSlotState State{};
            MemberInfo    Member{};
        };

        JoinResult     Result{};
        std::uint8_t   SlotID{};
        RoomTeam       Team{};
        std::string    Title{};
        std::uint32_t  MusicID{};
        MapInfo        Map{};
        GameMode       Mode{};
        Cx::Difficulty Difficulty{};
        std::uint8_t   SpeedID{};
        std::uint32_t  UserCount{};

        CollectionEnvelope<std::vector<SlotInfo>> Slots{8, 8};
    };

    inline Gx::Packet& operator<<(Gx::Packet& packet, const JoinRoomResponse::SlotInfo& slot)
    {
        packet << slot.Index << slot.State;
        if (slot.State == RoomSlotState::Occupied)
            packet << slot.Member;

        return packet;
    }

    inline Gx::Packet& operator>>(Gx::Packet& packet, JoinRoomResponse::SlotInfo& slot)
    {
        packet >> slot.Index >> slot.State;
        if (slot.State == RoomSlotState::Occupied)
            packet >> slot.Member;

        return packet;
    }
}
