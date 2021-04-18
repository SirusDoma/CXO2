#ifndef O2JAM_CHART_METADATA_HPP
#define O2JAM_CHART_METADATA_HPP

#include <SFML/System/String.hpp>

enum Difficulty
{
    Easy,
    Normal,
    Hard,
    Master
};

struct ChartMetadata
{
    sf::String Title;
    sf::String Artist;
    sf::String NoteDesigner;
    sf::String Genre;
    unsigned int Level;
};

#endif
