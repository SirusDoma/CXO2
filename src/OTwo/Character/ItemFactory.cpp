#include <Genode/Utilities/StringHelper.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/Models/Equipment.hpp>
#include <OTwo/IO/Loaders/Avatar/ItemLoader.hpp>

ItemFactory::ItemFactory(Gx::ResourceManager& sharedResources) :
    m_itemData(sharedResources.AddFromFile<ItemData>("Avatar/ItemData.json")),
    m_setInfoData(sharedResources.AddFromFile<SetInfoData>("Avatar/SetInfoData.json"))
{
    m_resources = &sharedResources;
}

std::unordered_map<EquipmentType, Item> ItemFactory::GetDefaultItems(const Gender& gender) const
{
    auto items = std::unordered_map<EquipmentType, Item>();
    const auto names = {
        "Avatar/default/Body.json",
        "Avatar/default/LeftArm.json",
        "Avatar/default/LeftHand.json",
        "Avatar/default/RightArm.json",
        "Avatar/default/RightHand.json"
    };

    auto equips = [&] (const std::initializer_list<std::string> equipments)
    {
        const auto loader = ItemLoader();
        for (auto& name : equipments)
        {
            auto item = m_resources->Instantiate<Item>(name, [&]
            {
                const auto ctx = Gx::ResourceContext(Gx::StringHelper::RemoveExtension(name), *m_resources, Gx::CacheMode::Reuse);
                return loader.LoadFromFile(name, ctx);
            });

            if (item)
                items[item->GetType()] = std::move(*item);
        }
    };

    for (const auto& name : names)
        equips({ name });

    if (gender == Gender::Male)
    {
        equips({
            "Avatar/default/male/Face.json",
            "Avatar/default/male/Hair.json",
            "Avatar/default/male/Top.json",
            "Avatar/default/male/Pants.json",
            "Avatar/default/male/Shoes.json"
        });
    }
    else if (gender == Gender::Female)
    {
        equips({
            "Avatar/default/female/Face.json",
            "Avatar/default/female/Hair.json",
            "Avatar/default/female/Top.json",
            "Avatar/default/female/Pants.json",
            "Avatar/default/female/Shoes.json"
        });
    }

    return items;
}

Item ItemFactory::Create(const unsigned int id) const
{
    const auto iterator = m_itemData.Items.find(id);
    if (iterator == m_itemData.Items.end())
        return {};

    const ItemMetadata metadata = iterator->second;
    const auto name   = fmt::format("Avatar/Items/{}", id);
    const auto loader = ItemLoader();

    return std::move(*m_resources->Instantiate<Item>(name, [&]
    {
        return loader.LoadFromMetadata(metadata, Gx::ResourceContext(name, *m_resources, Gx::CacheMode::Reuse));
    }));
}

const ItemMetadata& ItemFactory::GetItemMetadata(const unsigned int id) const
{
    if (const auto it = m_itemData.Items.find(id); it != m_itemData.Items.end())
        return it->second;

    return DefaultItemMetadata;
}

const ItemData& ItemFactory::GetItemData() const
{
    return m_itemData;
}

const SetInfoData& ItemFactory::GetSetInfoData() const
{
    return m_setInfoData;
}

void ItemFactory::ClearCache()
{
    m_resources->Clear();
}
