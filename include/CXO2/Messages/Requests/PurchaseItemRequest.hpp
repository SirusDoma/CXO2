#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct PurchaseItemRequest
    {
        static constexpr Cx::Command Command = RequestCommand::PurchaseItem;

        std::uint32_t ItemID{};
    };
}
