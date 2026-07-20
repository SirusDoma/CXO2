#pragma once

#include <CXO2/Models/Game.hpp>

#include <SFML/System/String.hpp>
#include <SFML/System/Time.hpp>

#include <cstdint>
#include <limits>
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

    struct MusicSelection
    {
        MusicSelection() = default;
        MusicSelection(const std::uint32_t value) :
            ID(value)
        {
            if (value <= std::numeric_limits<std::uint16_t>::max())
                return;

            constexpr auto maxBit = static_cast<std::uint8_t>(
                static_cast<int>(LevelCategory::Level1) |
                static_cast<int>(LevelCategory::Level2) |
                static_cast<int>(LevelCategory::Level3) |
                static_cast<int>(LevelCategory::Level4)
            );

            if (const auto randomBit = static_cast<std::uint8_t>((value >> 28) & 0xFF); randomBit >= 1 && randomBit <= maxBit)
            {
                ID     = value & 0xFF;
                Random = static_cast<LevelCategory>(randomBit);
            }
        }

        operator std::uint32_t() const
        {
            return ID | static_cast<std::uint32_t>(Random) << 28;
        }

        std::uint32_t ID{};
        LevelCategory Random{};
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

        std::string Source;
    };
}
