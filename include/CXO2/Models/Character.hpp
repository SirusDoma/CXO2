#pragma once

#include <cstdint>
#include <unordered_set>

namespace Cx
{
    enum class Gender : std::uint8_t
    {
        Female,
        Male,
        Any
    };

    enum class Role : std::uint8_t
    {
        Normal        = 0,
        Administrator = 1
    };

    using EquipmentSet = std::unordered_set<std::uint32_t>;
    using MusicList    = std::unordered_set<std::uint32_t>;
}
