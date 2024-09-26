#include <OTwo/Contexts/CartContext.hpp>

bool CartContext::AddEquipment(const unsigned int id)
{
    if (Contains(Mx::CartItemType::Equipment, id))
        return false;

    Record.Items.push_back({id, Mx::CartItemType::Equipment });
    return true;
}

bool CartContext::AddMusic(const unsigned int id)
{
    if (Contains(Mx::CartItemType::Music, id))
        return false;

    Record.Items.push_back({id, Mx::CartItemType::Music });
    return true;
}

bool CartContext::AddEquipmentSet(const unsigned int id)
{
    if (Contains(Mx::CartItemType::EquipmentSet, id))
        return false;

    Record.Items.push_back({id, Mx::CartItemType::EquipmentSet });
    return true;
}

bool CartContext::Remove(const std::size_t index)
{
    if (index >= Record.Items.size())
        return false;

    Record.Items.erase(Record.Items.begin() + index);
    return true;
}

const std::vector<Mx::CartItem>& CartContext::GetItems() const
{
    return Record.Items;
}

bool CartContext::Contains(Mx::CartItemType type, unsigned int id)
{
   return std::find_if(Record.Items.begin(), Record.Items.end(), [type, id] (const Mx::CartItem& item)
   {
       return item.ID == id && item.Type == type;
   }) != Record.Items.end();
}
