#pragma once

#include <SFML/System/String.hpp>
#include <SFML/System/Time.hpp>

#include <unordered_map>

enum class LevelCategory
{
    Level1 = 1 << 0,
    Level2 = 1 << 1,
    Level3 = 1 << 2,
    Level4 = 1 << 3
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
