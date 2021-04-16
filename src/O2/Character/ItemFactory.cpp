#include <O2/Character/ItemFactory.hpp>

void ItemFactory::Initialize(Gx::ResourceManager &sharedResources)
{
    m_resources = &sharedResources;
    m_itemData  = static_cast<ItemData*>(m_resources->LoadMetadata<ItemData>("Avatar/Itemdata.json"));
}

const std::map<Equipment::Type, Item *> ItemFactory::GetDefaultItems(const Character::Gender &gender)
{
    auto items = std::map<Equipment::Type, Item *>();
    if (!m_resources)
        return items;

    auto names = {
        "Avatar/default/Body.json",
        "Avatar/default/LeftArm.json",
        "Avatar/default/LeftHand.json",
        "Avatar/default/RightArm.json",
        "Avatar/default/RightHand.json",
    };

    for (auto name : names)
    {
        auto item = m_resources->Load<Item>(name);
        if (item)
            items[item->GetType()] = item;
    }

    std::string name = "";
    if (gender == Character::Gender::Male)
        name = "Avatar/default/Face_Male.json";
    else if (gender == Character::Gender::Female)
        name = "Avatar/default/Face_Female.json";

    if (!name.empty())
    {
        auto item = m_resources->Load<Item>(name);
        if (item)
            items[item->GetType()] = item;
    }

    return items;
}

Item *ItemFactory::GetItem(unsigned int id)
{
    if (!m_resources || !m_itemData)
        return nullptr;

    auto iterator = m_itemData->Items.find(id);
    if (iterator == m_itemData->Items.end())
        return nullptr;

    ItemMetadata metadata = iterator->second;
    return m_resources->Load<Item>("Avatar/Items/" + std::to_string(id) + ".json", iterator->second);
}
