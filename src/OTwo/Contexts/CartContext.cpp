#include <OTwo/Contexts/CartContext.hpp>

void CartContext::AddEquipment(const unsigned int id)
{
    if (Contains(Mx::CartItemType::Equipment, id))
        return;

    Record.Items.push_back({id, Mx::CartItemType::Equipment });
}

void CartContext::AddMusic(const unsigned int id)
{
    if (Contains(Mx::CartItemType::Music, id))
        return;

    Record.Items.push_back({id, Mx::CartItemType::Music });
}

void CartContext::AddEquipmentSet(const unsigned int id)
{
    if (Contains(Mx::CartItemType::EquipmentSet, id))
        return;

    Record.Items.push_back({id, Mx::CartItemType::EquipmentSet });
}

void CartContext::Remove(const std::size_t index)
{
    if (index >= Record.Items.size())
        return;

    Record.Items.erase(Record.Items.begin() + index);
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
