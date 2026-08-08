#pragma once

#include <CXO2/Events/Interceptable.hpp>
#include <CXO2/Avatar/Item.hpp>
#include <CXO2/Metadata/Avatar/ItemMetadata.hpp>
#include <CXO2/Models/Character.hpp>
#include <CXO2/Models/Equipment.hpp>
#include <CXO2/Models/Planet.hpp>
#include <CXO2/Models/Shop.hpp>

#include <Genode/Events/EventDispatcher.hpp>

#include <cstddef>

namespace Cx
{
    class StateItemShop;

    struct PurchaseItemResponse;
    struct SellItemResponse;

    enum class ItemShopEventKey
    {
        OnGenderFilterChange,
        OnPlanetFilterChange,
        OnMainCategoryChange,
        OnSubCategoryChange,
        OnItemPurchase,
        OnItemSell,
        OnPurchaseItemResponded,
        OnSellItemResponded
    };

    struct ItemShopGenderEventArgs : Interceptable
    {
        Cx::Gender& Gender;

        explicit ItemShopGenderEventArgs(Cx::Gender& gender) : Gender(gender) {}
    };

    struct ItemShopPlanetEventArgs : Interceptable
    {
        Cx::Planet& Planet;

        explicit ItemShopPlanetEventArgs(Cx::Planet& planet) : Planet(planet) {}
    };

    struct ItemShopMainCategoryEventArgs : Interceptable
    {
        ShopCategory& Category;

        explicit ItemShopMainCategoryEventArgs(ShopCategory& category) : Category(category) {}
    };

    struct ItemShopSubCategoryEventArgs : Interceptable
    {
        EquipmentType& Type;

        explicit ItemShopSubCategoryEventArgs(EquipmentType& type) : Type(type) {}
    };

    struct ItemShopPurchaseEventArgs : Interceptable
    {
        ItemMetadata& Metadata;

        explicit ItemShopPurchaseEventArgs(ItemMetadata& metadata) : Metadata(metadata) {}
    };

    struct ItemShopSellEventArgs : Interceptable
    {
        const Item& Target;
        std::size_t& SlotID;

        ItemShopSellEventArgs(const Item& target, std::size_t& slotID) :
            Target(target),
            SlotID(slotID)
        {
        }
    };

    struct ItemShopPurchaseResponseEventArgs : Interceptable
    {
        const PurchaseItemResponse& Response;
        const ItemMetadata& Metadata;

        ItemShopPurchaseResponseEventArgs(const PurchaseItemResponse& response, const ItemMetadata& metadata) :
            Response(response),
            Metadata(metadata)
        {
        }
    };

    struct ItemShopSellResponseEventArgs : Interceptable
    {
        const SellItemResponse& Response;

        explicit ItemShopSellResponseEventArgs(const SellItemResponse& response) : Response(response) {}
    };

    using ItemShopGenderEvent       = Gx::Event<ItemShopEventKey, StateItemShop, ItemShopGenderEventArgs>;
    using ItemShopPlanetEvent       = Gx::Event<ItemShopEventKey, StateItemShop, ItemShopPlanetEventArgs>;
    using ItemShopMainCategoryEvent = Gx::Event<ItemShopEventKey, StateItemShop, ItemShopMainCategoryEventArgs>;
    using ItemShopSubCategoryEvent  = Gx::Event<ItemShopEventKey, StateItemShop, ItemShopSubCategoryEventArgs>;
    using ItemShopPurchaseEvent     = Gx::Event<ItemShopEventKey, StateItemShop, ItemShopPurchaseEventArgs>;
    using ItemShopSellEvent         = Gx::Event<ItemShopEventKey, StateItemShop, ItemShopSellEventArgs>;
    using ItemShopPurchaseResponseEvent = Gx::Event<ItemShopEventKey, StateItemShop, ItemShopPurchaseResponseEventArgs>;
    using ItemShopSellResponseEvent     = Gx::Event<ItemShopEventKey, StateItemShop, ItemShopSellResponseEventArgs>;

    namespace ItemShopEvents
    {
        inline constexpr ItemShopGenderEvent       OnGenderFilterChange{ItemShopEventKey::OnGenderFilterChange};
        inline constexpr ItemShopPlanetEvent       OnPlanetFilterChange{ItemShopEventKey::OnPlanetFilterChange};
        inline constexpr ItemShopMainCategoryEvent OnMainCategoryChange{ItemShopEventKey::OnMainCategoryChange};
        inline constexpr ItemShopSubCategoryEvent  OnSubCategoryChange{ItemShopEventKey::OnSubCategoryChange};
        inline constexpr ItemShopPurchaseEvent     OnItemPurchase{ItemShopEventKey::OnItemPurchase};
        inline constexpr ItemShopSellEvent         OnItemSell{ItemShopEventKey::OnItemSell};
        inline constexpr ItemShopPurchaseResponseEvent OnPurchaseItemResponded{ItemShopEventKey::OnPurchaseItemResponded};
        inline constexpr ItemShopSellResponseEvent     OnSellItemResponded{ItemShopEventKey::OnSellItemResponded};
    }
}
