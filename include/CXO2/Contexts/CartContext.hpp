#pragma once

#include <CXO2/Models/Shop.hpp>

#include <functional>

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

        CartContext() = default;

        bool AddEquipment(unsigned int id);
        bool AddMusic(unsigned int id);
        bool AddEquipmentSet(unsigned int id);

        bool Remove(std::size_t index);

        bool Contains(CartItemType type, unsigned int id);
        const std::vector<CartItem>& GetItems() const;

        CheckoutType GetCheckoutType() const;
        void SetCheckoutType(CheckoutType checkoutType);

        void Clear();

    private:
        Cart m_cart;
        CheckoutType m_checkoutType;
    };
}
