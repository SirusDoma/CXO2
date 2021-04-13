#ifndef O2JAM_PLANET_HPP
#define O2JAM_PLANET_HPP

#include <vector>

namespace Planet
{
    enum class Planet
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
        MusicHall Hall;
        std::vector<ChannelInfo> Channels = std::vector<ChannelInfo>();
    };
}

#endif
