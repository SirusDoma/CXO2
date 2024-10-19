#ifndef O2JAM_CART_CONTEXT_HPP
#define O2JAM_CART_CONTEXT_HPP

#include <OTwo/IO/Serializable.hpp>
#include <OTwo/Serializable.g.hpp>

#include <functional>

class CartContext : public Serializable<Cart>
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

private:
    CheckoutType m_checkoutType;
};

#endif
