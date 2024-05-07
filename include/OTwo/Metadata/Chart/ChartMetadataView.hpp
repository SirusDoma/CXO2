#ifndef O2JAM_CHART_METADATA_HPP
#define O2JAM_CHART_METADATA_HPP

#include <SFML/System/String.hpp>
#include <SFML/System/Time.hpp>

enum class LevelCategory
{
    Level1 = 1 << 0,
    Level2 = 1 << 1,
    Level3 = 1 << 2,
    Level4 = 1 << 3
};

struct ChartMetadataView
{
    sf::String   ID;
    bool         New{};
    sf::String   Title;
    sf::String   Artist;
    sf::String   NoteDesigner;
    float        BPM{};
    sf::String   Genre;
    sf::String   Difficulty;
    unsigned int Level{};
    unsigned int NoteCount{};
    sf::Time     Duration;
    std::string  Source;

    LevelCategory GetLevelCategory() const
    {
        if (Level <= 5)
            return LevelCategory::Level1;
        else if (Level <= 9)
            return LevelCategory::Level2;
        else if (Level <= 13)
            return LevelCategory::Level3;

        return LevelCategory::Level4;
    }
};

#endif
