#ifndef O2JAM_ITEM_FACTORY_HPP
#define O2JAM_ITEM_FACTORY_HPP

#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Metadata/Avatar/ItemData.hpp>

#include <OTwo/Models/Character.hpp>
#include <OTwo/Models/Equipment.hpp>

#include <Genode/IO/ResourceManager.hpp>

#include <unordered_map>

class ItemFactory
{
public:
    ItemFactory() = default;
    explicit ItemFactory(Gx::ResourceManager &sharedResources);

    std::unordered_map<EquipmentType, Item*> GetDefaultItems(const Gender &gender) const;
    Item *GetItem(unsigned int id) const;

private:
    ItemData            *m_itemData;
    Gx::ResourceManager *m_resources;
};

#endif
