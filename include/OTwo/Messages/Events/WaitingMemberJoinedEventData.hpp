#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Messages/CollectionEnvelope.hpp>
#include <OTwo/Models/Room.hpp>

struct WaitingMemberJoinedEventData
{
    static constexpr Command Command = EventCommand::UserJoinWaiting;

    std::uint8_t ID{};
    std::string  Name{};
    std::int32_t Level{};
    Gender       Gender{};
    RoomTeam     Team{};
    bool         Ready{};

    CollectionEnvelope<std::unordered_set<std::uint32_t>> EquippedItemIDs{12, 12};
    CollectionEnvelope<std::unordered_set<std::uint32_t>, std::uint32_t> MusicIDs{};

};
