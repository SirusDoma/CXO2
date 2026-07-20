#pragma once

#include <cstdint>

namespace Cx
{
    enum class Currency : std::uint8_t
    {
        None = 0,
        Gem  = 1,
        Cash = 2
    };

    enum class CartItemType
    {
        Equipment,
        EquipmentSet,
        Music
    };
}
