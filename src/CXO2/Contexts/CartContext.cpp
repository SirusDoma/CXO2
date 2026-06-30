#include <CXO2/Contexts/CartContext.hpp>
#include <CXO2/Models/Shop.hpp>

#include <algorithm>

namespace Cx
{
    bool CartContext::AddEquipment(const unsigned int id)
    {
        if (Contains(CartItemType::Equipment, id))
            return false;

        m_cart.Items.push_back({id, CartItemType::Equipment });
        return true;
    }

    bool CartContext::AddMusic(const unsigned int id)
    {
        if (Contains(CartItemType::Music, id))
            return false;

        m_cart.Items.push_back({id, CartItemType::Music });
        return true;
    }

    bool CartContext::AddEquipmentSet(const unsigned int id)
    {
        if (Contains(CartItemType::EquipmentSet, id))
            return false;

        m_cart.Items.push_back({id, CartItemType::EquipmentSet });
        return true;
    }

    bool CartContext::Remove(const std::size_t index)
    {
        if (index >= m_cart.Items.size())
            return false;

        m_cart.Items.erase(m_cart.Items.begin() + index);
        return true;
    }

    const std::vector<CartItem>& CartContext::GetItems() const
    {
        return m_cart.Items;
    }

    CartContext::CheckoutType CartContext::GetCheckoutType() const
    {
        return m_checkoutType;
    }

    void CartContext::SetCheckoutType(const CheckoutType checkoutType)
    {
        m_checkoutType = checkoutType;
    }

    void CartContext::Clear()
    {
        m_cart.Items.clear();
    }

    bool CartContext::Contains(CartItemType type, unsigned int id)
    {
       return std::find_if(m_cart.Items.begin(), m_cart.Items.end(), [type, id] (const CartItem& item)
       {
           return item.ID == id && item.Type == type;
       }) != m_cart.Items.end();
    }
}
