#pragma once

#include <SFML/System/String.hpp>

#include <cstdint>
#include <unordered_set>
#include <vector>

namespace Cx
{
    enum class Gender : std::uint8_t
    {
        Female,
        Male,
        Any
    };

    enum class Role : std::uint8_t
    {
        Normal        = 0,
        Administrator = 1
    };

    struct CharacterInfo
    {
        struct ItemInfo
        {
            // ReSharper disable once CppNonExplicitConvertingConstructor
            ItemInfo(const std::uint32_t id) : ID(id) {}

            // ReSharper disable once CppNonExplicitConversionOperator
            operator std::uint32_t() const { return ID;}

            std::uint32_t ID{};
            std::uint32_t Quantity{1};
        };

        struct RankStatsInfo
        {
            std::int32_t Rank{};
            std::int32_t Wins{};
            std::int32_t Loses{};
            std::int32_t Draws{};
        };

        struct WalletInfo
        {
            std::uint32_t Gem{};
            std::uint32_t Cash{}; // a.k.a point a.k.a ePoint a.k.a eP
        };

        using EquipmentSet = std::unordered_set<std::uint32_t>;
        using MusicList    = std::unordered_set<std::uint32_t>;
        using ItemList     = std::vector<ItemInfo>;

        sf::String    Name;
        Cx::Gender    Gender;
        Cx::Role      Role;
        std::int32_t  Level;
        std::int32_t  Experience;
        RankStatsInfo RankStats;
        WalletInfo    Wallet;

        EquipmentSet EquippedItemIDs;
        ItemList     Inventory;
        MusicList    MusicIDs;
    };
}
