#include <O2/Character/ItemFactory.hpp>
#include <iostream>

ItemFactory::ItemFactory()
{
}

ItemFactory::ItemFactory(Gx::ResourceManager &sharedResources)
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
        "Avatar/default/RightHand.json"
    };

    for (auto name : names)
    {
        auto item = m_resources->Load<Item>(name);
        if (item)
            items[item->GetType()] = item;
    }

    auto apply = [&] (std::initializer_list<std::string> equipments)
    {
        for (auto name : equipments)
        {
            auto item = m_resources->Load<Item>(name);
            if (item)
                items[item->GetType()] = item;
        }
    };

    if (gender == Character::Gender::Male)
    {
        apply({
            "Avatar/default/Male/Face.json",
            "Avatar/default/Male/Hair.json",
            "Avatar/default/Male/Jacket.json",
            "Avatar/default/Male/Pants.json",
            "Avatar/default/Male/Shoes.json"
        });
    }
    else if (gender == Character::Gender::Female)
    {
        apply({
            "Avatar/default/Female/Face.json",
            "Avatar/default/Female/Hair.json",
            "Avatar/default/Female/Jacket.json",
            "Avatar/default/Female/Pants.json",
            "Avatar/default/Female/Shoes.json"
        });
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
