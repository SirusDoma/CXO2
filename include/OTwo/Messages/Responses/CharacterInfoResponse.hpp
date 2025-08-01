#pragma once

#include <OTwo/Models/Character.hpp>
#include <OTwo/Messages/CollectionEnvelope.hpp>

#include <string>
#include <cstdint>
#include <unordered_set>

struct CharacterInfoResponse
{
    static constexpr Command Command = ResponseCommand::GetCharacterInfo;

    std::uint32_t Suspended{};
    std::string   Name{};
    ::Gender      Gender{};
    std::uint32_t Gem{};
    std::uint32_t Point{};
    std::int32_t  Level{};
    std::int32_t  Wins{};
    std::int32_t  Loses{};
    std::int32_t  Draws{};
    std::int32_t  Experience{};
    ::Role        Role{};

    CollectionEnvelope<std::unordered_set<std::uint32_t>> EquippedItemIDs{12, 12};
    CollectionEnvelope<std::vector<std::uint32_t>> Inventory{30, 30};
    CollectionEnvelope<std::vector<std::uint32_t>, std::uint32_t> AttributiveItems{};
};
