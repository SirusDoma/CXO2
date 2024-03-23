#ifndef O2JAM_ITEM_FACTORY_HPP
#define O2JAM_ITEM_FACTORY_HPP

#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Metadata/Avatar/ItemData.hpp>

#include <OTwo/Data/Character.hpp>
#include <OTwo/Data/Equipment.hpp>

#include <Genode/System/Module.hpp>
#include <Genode/IO/ResourceManager.hpp>

#include <unordered_map>

class ItemFactory : public Gx::Module
{
public:
    ItemFactory();
    explicit ItemFactory(Gx::ResourceManager &sharedResources);

    std::unordered_map<EquipmentType, Item*> GetDefaultItems(const Gender &gender);
    Item *GetItem(unsigned int id);

private:
    ItemData            *m_itemData;
    Gx::ResourceManager *m_resources;
};

#endif
