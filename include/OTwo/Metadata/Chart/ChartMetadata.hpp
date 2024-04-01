#ifndef O2JAM_CHART_METADATA_HPP
#define O2JAM_CHART_METADATA_HPP

#include <string>

#include <SFML/System/Time.hpp>

enum Difficulty
{
    Easy,
    Normal,
    Hard,
    Master
};

struct ChartMetadata
{
    unsigned int ID;
    bool         New;
    std::string  Title;
    std::string  Artist;
    std::string  NoteDesigner;
    std::string  Genre;
    float        BPM;
    unsigned int LevelEx;
    unsigned int LevelNx;
    unsigned int LevelHx;
    unsigned int NoteCountEx;
    unsigned int NoteCountNx;
    unsigned int NoteCountHx;
    sf::Time     Duration;
};

#endif
