#pragma once

#include <OTwo/Messages/Commands.hpp>

struct PurchaseItemRequest
{
    static constexpr Command Command = RequestCommand::PurchaseItem;

    std::uint32_t ItemID{};
};
