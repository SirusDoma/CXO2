#include <O2/Character/ItemFactory.hpp>

void ItemFactory::Initialize(Gx::ResourceManager &sharedResources)
{
    m_resources = &sharedResources;
    m_itemData  = static_cast<ItemData*>(m_resources->LoadMetadata<ItemData>("Avatar/Itemdata.json"));
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
