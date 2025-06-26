#pragma once

#include <vector>

enum class Planet : std::uint8_t
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

enum class MusicHall : std::uint8_t
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
