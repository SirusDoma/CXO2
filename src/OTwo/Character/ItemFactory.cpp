#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/Data/Character.hpp>
#include <OTwo/Data/Equipment.hpp>
#include <OTwo/IO/Loaders/Avatar/ItemLoader.hpp>

ItemFactory::ItemFactory(Gx::ResourceManager &sharedResources)
{
    m_resources = &sharedResources;
    m_itemData  = &m_resources->AddFromFile<ItemData>("Avatar/Itemdata.json");
}

std::unordered_map<EquipmentType, Item*> ItemFactory::GetDefaultItems(const Gender &gender) const
{
    auto items = std::unordered_map<EquipmentType, Item*>();
    if (!m_resources)
        return items;

    const auto names = {
        "Avatar/default/Body.json",
        "Avatar/default/LeftArm.json",
        "Avatar/default/LeftHand.json",
        "Avatar/default/RightArm.json",
        "Avatar/default/RightHand.json"
    };

    for (const auto name : names)
    {
        auto& item = m_resources->AddFromFile<Item>(name);
        items[item.GetType()] = &item;
    }

    auto apply = [&] (const std::initializer_list<std::string> equipments)
    {
        for (auto& name : equipments)
        {
            auto& item = m_resources->AddFromFile<Item>(name);
            items[item.GetType()] = &item;
        }
    };

    if (gender == Gender::Male)
    {
        apply({
            "Avatar/default/male/Face.json",
            "Avatar/default/male/Hair.json",
            "Avatar/default/male/Jacket.json",
            "Avatar/default/male/Pants.json",
            "Avatar/default/male/Shoes.json"
        });
    }
    else if (gender == Gender::Female)
    {
        apply({
            "Avatar/default/female/Face.json",
            "Avatar/default/female/Hair.json",
            "Avatar/default/female/Jacket.json",
            "Avatar/default/female/Pants.json",
            "Avatar/default/female/Shoes.json"
        });
    }

    return items;
}

Item *ItemFactory::GetItem(const unsigned int id) const
{
    if (!m_resources || !m_itemData)
        return nullptr;

    const auto iterator = m_itemData->Items.find(id);
    if (iterator == m_itemData->Items.end())
        return nullptr;

    const ItemMetadata metadata = iterator->second;
    const auto loader = ItemLoader();

    const auto name = "Avatar/Items/" + std::to_string(id);
    const auto ctx  = Gx::ResourceContext(name, *m_resources, Gx::CacheMode::Reuse);

    return &m_resources->AddFromDeserializer<Item>(name, [&] () { return loader.LoadFromMetadata(metadata, ctx); }, ctx.GetCacheMode());
}
