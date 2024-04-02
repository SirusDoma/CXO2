#ifndef O2JAM_CHART_METADATA_HPP
#define O2JAM_CHART_METADATA_HPP

#include <SFML/System/String.hpp>
#include <SFML/System/Time.hpp>

enum Difficulty
{
    Easy,
    Normal,
    Hard,
    Master
};

enum OhmLevel
{
    Level1 = 1 << 0,
    Level2 = 1 << 1,
    Level3 = 1 << 2,
    Level4 = 1 << 3
};

struct ChartMetadata
{
    sf::String   ID;
    bool         New;
    sf::String   Title;
    sf::String   Artist;
    sf::String   NoteDesigner;
    float        BPM;
    sf::String   Genre;
    unsigned int Level;
    unsigned int NoteCount;
    sf::Time     Duration;

    OhmLevel GetOhmLevel() const
    {
        if (Level <= 5)
            return OhmLevel::Level1;
        else if (Level <= 9)
            return OhmLevel::Level2;
        else if (Level <= 13)
            return OhmLevel::Level3;

        return OhmLevel::Level4;
    }
};

#endif
