#pragma once

#include <CXO2/Avatar/Item.hpp>
#include <CXO2/Metadata/Avatar/ItemData.hpp>
#include <CXO2/Metadata/Avatar/SetInfoData.hpp>

#include <CXO2/Models/Equipment.hpp>
#include <CXO2/Models/Character.hpp>

#include <Genode/IO/ResourceManager.hpp>

#include <unordered_map>

namespace Cx
{
    class ItemFactory
    {
    public:
        explicit ItemFactory(
            Gx::ResourceManager& sharedResources,
            const std::string& itemDataFileName,
            const std::string& setInfoDataFileName
        );

        std::unordered_map<EquipmentType, Item> GetDefaultItems(const Gender& gender) const;
        Item Create(unsigned int id) const;

        const ItemMetadata& GetItemMetadata(unsigned int id) const;

        const ItemData& GetItemData() const;
        const SetInfoData& GetSetInfoData() const;

        void ClearCache();

    private:
        Gx::ResourceManager& m_resources;
        ItemData&            m_itemData;
        SetInfoData*         m_setInfoData;
    };
}
