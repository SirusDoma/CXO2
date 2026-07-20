#pragma once

#include <CXO2/Models/Shop.hpp>

#include <functional>
#include <vector>

namespace Cx
{
    class CartContext
    {
    public:
        enum class CheckoutType
        {
            Item,
            Music
        };

        struct Item
        {
            unsigned int ID;
            CartItemType Type;
        };

        CartContext() = default;

        bool AddEquipment(unsigned int id);
        bool AddMusic(unsigned int id);
        bool AddEquipmentSet(unsigned int id);

        bool Remove(std::size_t index);

        bool Contains(CartItemType type, unsigned int id);
        const std::vector<Item>& GetItems() const;

        CheckoutType GetCheckoutType() const;
        void SetCheckoutType(CheckoutType checkoutType);

        void Clear();

    private:
        std::vector<Item> m_items;
        CheckoutType m_checkoutType;
    };
}
