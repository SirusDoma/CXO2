#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct SellItemRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SellItem;

        std::uint32_t SlotID{};
    };
}
