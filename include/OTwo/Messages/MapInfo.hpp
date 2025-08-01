#pragma once

#include <cstdint>

struct MapInfo
{
    static constexpr std::uint8_t RandomID = 0x80;

    std::uint8_t  Prefix{};
    std::uint16_t Reserved{};
    std::uint8_t  Suffix{};

    std::uint8_t GetMapID() const
    {
        if (Suffix == RandomID)
            return 0;

        return Prefix;
    }

    std::uint8_t GetRandomizedMap() const
    {
        if (Suffix == RandomID)
            return Prefix;

        return 0;
    }
};
