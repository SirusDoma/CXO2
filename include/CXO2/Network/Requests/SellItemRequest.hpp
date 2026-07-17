#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct SellItemRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SellItem;

        std::uint32_t SlotID{};
    };
}
