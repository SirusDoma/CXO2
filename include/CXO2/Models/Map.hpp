#pragma once

#include <cstdint>

namespace Cx
{
    struct Map
    {
        static constexpr std::uint8_t RandomID = 0x80;

        static Map From(const std::uint32_t value)
        {
            auto map = Map{};
            map.ID     = static_cast<std::uint8_t>(value & 0xFFFF);
            map.Random = (value >> 24) == RandomID;

            return map;
        }

        static Map Of(const std::uint8_t id, const bool random = false)
        {
            auto map = Map{};
            map.ID     = id;
            map.Random = random;

            return map;
        }

        // ReSharper disable once CppNonExplicitConversionOperator
        operator std::uint32_t() const
        {
            return Random ? (static_cast<std::uint32_t>(RandomID) << 24) | ID : ID;
        }

        std::uint8_t ID{};
        bool Random{};
    };
}
