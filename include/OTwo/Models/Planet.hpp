#ifndef O2JAM_MODELS_PLANET_HPP
#define O2JAM_MODELS_PLANET_HPP

#include <OTwo/Serializable.g.hpp>
#include <vector>

struct ServerChannel
{
    int ID            = 0;
    int Population    = 0;
    int MaxPopulation = 100;
};

struct PlanetInfo
{
    ::Planet Planet = Planet::O2Planet;
    MusicHall Hall = MusicHall::None;
    std::vector<ServerChannel> Channels;
};

#endif
