#include <OTwo/Contexts/CartContext.hpp>

bool CartContext::AddEquipment(const unsigned int id)
{
    if (Contains(CartItemType::Equipment, id))
        return false;

    Record.Items.push_back({id, CartItemType::Equipment });
    return true;
}

bool CartContext::AddMusic(const unsigned int id)
{
    if (Contains(CartItemType::Music, id))
        return false;

    Record.Items.push_back({id, CartItemType::Music });
    return true;
}

bool CartContext::AddEquipmentSet(const unsigned int id)
{
    if (Contains(CartItemType::EquipmentSet, id))
        return false;

    Record.Items.push_back({id, CartItemType::EquipmentSet });
    return true;
}

bool CartContext::Remove(const std::size_t index)
{
    if (index >= Record.Items.size())
        return false;

    Record.Items.erase(Record.Items.begin() + index);
    return true;
}

const std::vector<CartItem>& CartContext::GetItems() const
{
    return Record.Items;
}

bool CartContext::Contains(CartItemType type, unsigned int id)
{
   return std::find_if(Record.Items.begin(), Record.Items.end(), [type, id] (const CartItem& item)
   {
       return item.ID == id && item.Type == type;
   }) != Record.Items.end();
}
