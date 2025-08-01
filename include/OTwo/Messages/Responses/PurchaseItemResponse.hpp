#pragma once

#include <OTwo/Messages/Commands.hpp>

enum class PurchaseItemResult : std::uint32_t
{
    Success           = 0x00000000, // 0
    InsufficientMoney = 0x00000001, // 1
    InventoryFull     = 0x00000002  // 2
};

struct PurchaseItemResponse
{
    static constexpr Command Command = ResponseCommand::PurchaseItem;

    PurchaseItemResult ResultCode{};
    std::uint32_t Gem{};
    std::uint32_t Cash{};
    std::uint32_t SlotID{};
    std::uint32_t ItemID{};
};
