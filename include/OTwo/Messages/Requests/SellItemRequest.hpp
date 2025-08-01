#pragma once

#include <OTwo/Messages/Commands.hpp>

struct SellItemRequest
{
    static constexpr Command Command = RequestCommand::SellItem;

    std::uint32_t SlotID{};
};
