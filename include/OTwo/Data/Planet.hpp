#ifndef O2JAM_PLANET_DATA_HPP
#define O2JAM_PLANET_DATA_HPP

#include <vector>

enum class Planet
{
    Unknown,
    O2Planet,
    Aqua,
    Eliten,
    Graffiti,
    Bikini,
    Crush,
    Wonderland,
    Meganut,
    Crystal,
    Draconic,
    Event
};

enum class MusicHall
{
    None,
    Melpomin,
    Thalo,
    Euta,
    Kalliope,
    Kleo,
    Philix
};
struct Channel
{
    int ID            = 0;
    int Population    = 0;
    int MaxPopulation = 100;
};

struct PlanetData
{
    ::Planet Planet = Planet::O2Planet;
    MusicHall Hall = MusicHall::None;
    std::vector<Channel> Channels;
};

#endif
