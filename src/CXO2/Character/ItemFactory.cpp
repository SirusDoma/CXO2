#include <Genode/Utilities/StringHelper.hpp>
#include <CXO2/O2.hpp>
#include <CXO2/Avatar/ItemFactory.hpp>
#include <CXO2/Models/Equipment.hpp>
#include <CXO2/IO/Loaders/Avatar/ItemLoader.hpp>

namespace Cx
{
    ItemFactory::ItemFactory(
        Gx::ResourceManager& sharedResources,
        const std::string& itemDataFileName,
        const std::string& setInfoDataFileName
    ) :
        m_resources(sharedResources),
        m_itemData(sharedResources.AddFromFile<ItemData>(itemDataFileName)),
        m_setInfoData(setInfoDataFileName.empty() ? nullptr : &sharedResources.AddFromFile<SetInfoData>(setInfoDataFileName))
    {
    }

    std::unordered_map<EquipmentType, Item> ItemFactory::GetDefaultItems(const Gender& gender) const
    {
        auto items = std::unordered_map<EquipmentType, Item>();
        if (O2::InInteropMode(InteropMode::Avatar))
        {
            const auto ids = { 30, 31, 32, 33, 34, gender == Gender::Female ? 36 : 35 };
            for (const auto id : ids)
            {
                if (auto item = Create(id); item.GetID() != 0)
                    items[item.GetType()] = std::move(item);
            }

            return items;
        }

        auto names = std::vector<std::string>
        {
            "Avatar/default/Body.json",
            "Avatar/default/LeftArm.json",
            "Avatar/default/LeftHand.json",
            "Avatar/default/RightArm.json",
            "Avatar/default/RightHand.json"
        };

        if (gender == Gender::Male || gender == Gender::Female)
        {
            const auto directory = gender == Gender::Male ? "male" : "female";
            for (const auto part : { "Face", "Hair", "Top", "Pants", "Shoes" })
                names.push_back(fmt::format("Avatar/default/{}/{}.json", directory, part));
        }

        const auto loader = ItemLoader();
        for (const auto& name : names)
        {
            if (!Gx::FileSystem::Contains(name))
                continue;

            const auto item = m_resources.Instantiate<Item>(name, [&]
            {
                const auto ctx = Gx::ResourceContext(Gx::StringHelper::RemoveExtension(name), m_resources, Gx::CacheMode::Reuse);
                return loader.LoadFromFile(name, ctx);
            });

            if (item)
                items[item->GetType()] = std::move(*item);
        }

        return items;
    }

    Item ItemFactory::Create(const unsigned int id) const
    {
        const auto iterator = m_itemData.Items.find(id);
        if (iterator == m_itemData.Items.end())
            return {};

        const auto name = fmt::format("Avatar/Items/{}", id);
        const auto item = m_resources.Instantiate<Item>(name, [&]
        {
            return ItemLoader().LoadFromMetadata(iterator->second, Gx::ResourceContext(name, m_resources, Gx::CacheMode::Reuse));
        });

        return std::move(*item);
    }

    const ItemMetadata& ItemFactory::GetItemMetadata(const unsigned int id) const
    {
        static const auto defaultMetadata = ItemMetadata();
        if (const auto iterator = m_itemData.Items.find(id); iterator != m_itemData.Items.end())
            return iterator->second;

        return defaultMetadata;
    }

    const ItemData& ItemFactory::GetItemData() const
    {
        return m_itemData;
    }

    const SetInfoData& ItemFactory::GetSetInfoData() const
    {
        static const auto empty = SetInfoData();
        if (!m_setInfoData)
            return empty;

        return *m_setInfoData;
    }

    void ItemFactory::ClearCache()
    {
        m_resources.Clear();
    }
}
