#pragma once

#include <CXO2/Models/Game.hpp>
#include <CXO2/Models/Shop.hpp>

#include <SFML/System/String.hpp>
#include <SFML/System/Time.hpp>

#include <cstdint>
#include <limits>
#include <map>
#include <unordered_map>

namespace Cx
{
    enum class LevelCategory
    {
        Level1 = 1 << 0,
        Level2 = 1 << 1,
        Level3 = 1 << 2,
        Level4 = 1 << 3
    };

    struct MusicID
    {
        static MusicID From(const std::uint32_t value)
        {
            auto id = MusicID{};
            id.Value = value;

            if (value <= std::numeric_limits<std::uint16_t>::max())
                return id;

            constexpr auto maxBit = static_cast<std::uint8_t>(
                static_cast<int>(LevelCategory::Level1) |
                static_cast<int>(LevelCategory::Level2) |
                static_cast<int>(LevelCategory::Level3) |
                static_cast<int>(LevelCategory::Level4)
            );

            if (const auto randomBit = static_cast<std::uint8_t>((value >> 28) & 0xFF); randomBit >= 1 && randomBit <= maxBit)
            {
                id.Value       = value & 0xFF;
                id.RandomLevel = static_cast<LevelCategory>(randomBit);
            }

            return id;
        }

        static MusicID Of(const std::uint32_t value, const LevelCategory random = static_cast<LevelCategory>(0))
        {
            auto id = MusicID{};
            id.Value  = value;
            id.RandomLevel = random;

            return id;
        }

        // ReSharper disable once CppNonExplicitConversionOperator
        operator std::uint32_t() const
        {
            return Value | static_cast<std::uint32_t>(RandomLevel) << 28;
        }

        std::uint32_t Value{};
        LevelCategory RandomLevel{};
    };

    enum class MusicStatus
    {
        Playable,
        Missing,
        Corrupted,
        InvalidFormat,
        Unacquired
    };

    struct ChartMetadata
    {
        std::uint32_t ID{};
        bool          New{};
        sf::String    Title;
        sf::String    Artist;
        sf::String    NoteDesigner;
        float         BPM{};
        sf::String    Genre;

        std::unordered_map<Difficulty, unsigned int>  Levels{};
        std::unordered_map<Difficulty, unsigned int>  NoteCounts{};
        std::unordered_map<Difficulty, sf::Time>      Durations{};
        std::unordered_map<Currency, unsigned int>    Prices{};

        std::string Source;
        MusicStatus Status{};
    };
}
