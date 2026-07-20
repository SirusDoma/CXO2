#pragma once

#include <cstdint>

namespace Cx
{
    struct Map
    {
        static constexpr std::uint8_t RandomID = 0x80;

        Map() = default;
        Map(const std::uint32_t value) :
            ID(static_cast<std::uint8_t>(value & 0xFFFF)),
            Random((value >> 24) == RandomID)
        {
        }

        operator std::uint32_t() const
        {
            return Random ? (static_cast<std::uint32_t>(RandomID) << 24) | ID : ID;
        }

        std::uint8_t GetMapID() const
        {
            return Random ? 0 : ID;
        }

        std::uint8_t GetRandomizedMap() const
        {
            return Random ? ID : 0;
        }

        std::uint8_t ID{};
        bool Random{};
    };
}
