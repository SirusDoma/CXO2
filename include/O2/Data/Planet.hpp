#ifndef O2JAM_PLANET_HPP
#define O2JAM_PLANET_HPP

#include <vector>

enum Planet
{
    None,
    Melpomin,
    Thalo,
    Euta,
    Kaliope,
    Kleo,
    Philix
};

struct ChannelInfo
{
    int Number        = 0;
    int Population    = 0;
    int MaxPopulation = 100;
};

struct PlanetInfo
{
    Planet Planet;
    std::vector<ChannelInfo> Channels = std::vector<ChannelInfo>();
};

#endif
