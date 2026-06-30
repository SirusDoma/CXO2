#pragma once

#include <cstdint>

namespace Cx
{
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
}
