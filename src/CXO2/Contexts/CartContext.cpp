#include <CXO2/Contexts/CartContext.hpp>
#include <CXO2/Models/Shop.hpp>

#include <algorithm>

namespace Cx
{
    bool CartContext::AddEquipment(const unsigned int id)
    {
        if (Contains(CartItemType::Equipment, id))
            return false;

        m_items.push_back({id, CartItemType::Equipment });
        return true;
    }

    bool CartContext::AddMusic(const unsigned int id)
    {
        if (Contains(CartItemType::Music, id))
            return false;

        m_items.push_back({id, CartItemType::Music });
        return true;
    }

    bool CartContext::AddEquipmentSet(const unsigned int id)
    {
        if (Contains(CartItemType::EquipmentSet, id))
            return false;

        m_items.push_back({id, CartItemType::EquipmentSet });
        return true;
    }

    bool CartContext::Remove(const std::size_t index)
    {
        if (index >= m_items.size())
            return false;

        m_items.erase(m_items.begin() + index);
        return true;
    }

    const std::vector<CartContext::Item>& CartContext::GetItems() const
    {
        return m_items;
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
        m_items.clear();
    }

    bool CartContext::Contains(CartItemType type, unsigned int id)
    {
       return std::find_if(m_items.begin(), m_items.end(), [type, id] (const Item& item)
       {
           return item.ID == id && item.Type == type;
       }) != m_items.end();
    }
}
