#ifndef O2JAM_CHART_METADATA_HPP
#define O2JAM_CHART_METADATA_HPP

#include <string>

enum Difficulty
{
    Easy,
    Normal,
    Hard,
    Master
};

struct ChartMetadata
{
    std::string Title;
    std::string Artist;
    std::string NoteDesigner;
    std::string Genre;
    unsigned int Level;
    bool New;
};

#endif
