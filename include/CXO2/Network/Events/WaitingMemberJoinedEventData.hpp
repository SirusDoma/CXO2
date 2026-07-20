#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Network/CollectionEnvelope.hpp>
#include <CXO2/Models/Room.hpp>
#include <CXO2/Models/Character.hpp>

namespace Cx
{
    struct WaitingMemberJoinedEventData
    {
        static constexpr Cx::Command Command = EventCommand::UserJoinWaiting;

        std::uint8_t ID{};
        std::string  Name{};
        std::int32_t Level{};
        Cx::Gender   Gender{};
        Room::Team   Team{};
        bool         Ready{};

        CollectionEnvelope<std::unordered_set<std::uint32_t>> EquippedItemIDs{12, 12};
        CollectionEnvelope<std::unordered_set<std::uint32_t>, std::uint32_t> MusicIDs{};

    };
}
