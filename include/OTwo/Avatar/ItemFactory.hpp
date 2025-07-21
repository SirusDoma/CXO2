#pragma once

#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Metadata/Avatar/ItemData.hpp>
#include <OTwo/Metadata/Avatar/SetInfoData.hpp>

#include <OTwo/Models/Equipment.hpp>
#include <OTwo/Models/Character.hpp>

#include <Genode/IO/ResourceManager.hpp>

#include <unordered_map>

class ItemFactory
{
public:
    explicit ItemFactory(Gx::ResourceManager& sharedResources, const std::string& itemDataFileName, const std::string& setInfoDataFileName);

    std::unordered_map<EquipmentType, Item> GetDefaultItems(const Gender& gender) const;
    Item Create(unsigned int id) const;

    const ItemMetadata& GetItemMetadata(unsigned int id) const;

    const ItemData& GetItemData() const;
    const SetInfoData& GetSetInfoData() const;

    void ClearCache();

private:
    const ItemMetadata DefaultItemMetadata{};

    ItemData& m_itemData;
    SetInfoData* m_setInfoData;
    Gx::ResourceManager* m_resources;
};
