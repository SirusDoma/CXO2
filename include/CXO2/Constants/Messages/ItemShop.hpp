#pragma once

namespace Cx::Constants::Messages::ItemShop
{
    constexpr static auto NICKNAME                = U" Lv.{}:{}";
    constexpr static auto NOTHING_SELECTED        = "No item selected.";
    constexpr static auto NOT_ENOUGH_MONEY        = "Not enough money to buy.";
    constexpr static auto WRONG_GENDER            = "This item is for other gender.";
    constexpr static auto STANDARD_FACE_LOCKED    = "Standard face is neither purchasable nor sellable.";
    constexpr static auto SELL_CONFIRM            = "Are you sure to sell the item you selected?";
    constexpr static auto VACANT_SLOT_REQUIRED    = "At least one vacant slot is required."; // Unused

    namespace PurchaseResponse
    {
        constexpr static auto NOT_ENOUGH_MONEY = "You need more money.";
        constexpr static auto NO_VACANT_SLOT   = "No more room in your Bag and Apply window.";
    }
}
