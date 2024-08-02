#ifndef O2JAM_MODELS_PLANET_HPP
#define O2JAM_MODELS_PLANET_HPP

#include <vector>
#include <Genode/System/Primitives.hpp>

enum class Planet : Gx::Uint8
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

enum class MusicHall : Gx::Uint8
{
    None,
    Melpomin,
    Thalo,
    Euta,
    Kalliope,
    Kleo,
    Philix
};

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
