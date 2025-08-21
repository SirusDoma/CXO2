#pragma once

#include <OTwo/Messages/Commands.hpp>

enum class SellItemResult : std::uint32_t
{
    Success = 0x00000000, // 0
    Failed  = 0x00000001, // 1
};

struct SellItemResponse
{
    static constexpr ::Command Command = ResponseCommand::SellItem;

    SellItemResult Result{};
    std::uint32_t  Gem{};
    std::uint32_t  Cash{};
    std::uint32_t  SlotID{};
};
