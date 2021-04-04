#ifndef O2JAM_METADATA_CHARTMETADATA_HPP
#define O2JAM_METADATA_CHARTMETADATA_HPP

#include <SFML/System/String.hpp>

enum Difficulty
{
    Easy   = 0,
    Normal = 1,
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
