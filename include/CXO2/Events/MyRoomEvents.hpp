#pragma once

#include <CXO2/Events/Interceptable.hpp>
#include <CXO2/Avatar/Item.hpp>

#include <Genode/Events/EventDispatcher.hpp>

#include <cstddef>

namespace Cx
{
    class StateMyRoom;

    struct EquipItemResponse;
    struct SellItemResponse;

    enum class MyRoomEventKey
    {
        OnItemEquip,
        OnItemUnequip,
        OnItemSell,
        OnEquipItemResponded,
        OnUnequipItemResponded,
        OnSellItemResponded
    };

    struct MyRoomEquipEventArgs : Interceptable
    {
        const Item& Target;
        std::size_t& SlotID;

        MyRoomEquipEventArgs(const Item& target, std::size_t& slotID) :
            Target(target),
            SlotID(slotID)
        {
        }
    };

    struct MyRoomUnequipEventArgs : Interceptable
    {
        const Item& Target;
        std::size_t& SlotID;

        MyRoomUnequipEventArgs(const Item& target, std::size_t& slotID) :
            Target(target),
            SlotID(slotID)
        {
        }
    };

    struct MyRoomSellEventArgs : Interceptable
    {
        const Item& Target;
        std::size_t& SlotID;

        MyRoomSellEventArgs(const Item& target, std::size_t& slotID) :
            Target(target),
            SlotID(slotID)
        {
        }
    };

    struct MyRoomEquipResponseEventArgs : Interceptable
    {
        const EquipItemResponse& Response;

        explicit MyRoomEquipResponseEventArgs(const EquipItemResponse& response) : Response(response) {}
    };

    struct MyRoomUnequipResponseEventArgs : Interceptable
    {
        const EquipItemResponse& Response;

        explicit MyRoomUnequipResponseEventArgs(const EquipItemResponse& response) : Response(response) {}
    };

    struct MyRoomSellResponseEventArgs : Interceptable
    {
        const SellItemResponse& Response;

        explicit MyRoomSellResponseEventArgs(const SellItemResponse& response) : Response(response) {}
    };

    using MyRoomEquipEvent           = Gx::Event<MyRoomEventKey, StateMyRoom, MyRoomEquipEventArgs>;
    using MyRoomUnequipEvent         = Gx::Event<MyRoomEventKey, StateMyRoom, MyRoomUnequipEventArgs>;
    using MyRoomSellEvent            = Gx::Event<MyRoomEventKey, StateMyRoom, MyRoomSellEventArgs>;
    using MyRoomEquipResponseEvent   = Gx::Event<MyRoomEventKey, StateMyRoom, MyRoomEquipResponseEventArgs>;
    using MyRoomUnequipResponseEvent = Gx::Event<MyRoomEventKey, StateMyRoom, MyRoomUnequipResponseEventArgs>;
    using MyRoomSellResponseEvent    = Gx::Event<MyRoomEventKey, StateMyRoom, MyRoomSellResponseEventArgs>;

    namespace MyRoomEvents
    {
        inline constexpr MyRoomEquipEvent           OnItemEquip{MyRoomEventKey::OnItemEquip};
        inline constexpr MyRoomUnequipEvent         OnItemUnequip{MyRoomEventKey::OnItemUnequip};
        inline constexpr MyRoomSellEvent            OnItemSell{MyRoomEventKey::OnItemSell};
        inline constexpr MyRoomEquipResponseEvent   OnEquipItemResponded{MyRoomEventKey::OnEquipItemResponded};
        inline constexpr MyRoomUnequipResponseEvent OnUnequipItemResponded{MyRoomEventKey::OnUnequipItemResponded};
        inline constexpr MyRoomSellResponseEvent    OnSellItemResponded{MyRoomEventKey::OnSellItemResponded};
    }
}
