#ifndef O2JAM_ITEM_FACTORY_HPP
#define O2JAM_ITEM_FACTORY_HPP

#include <OTwo/Metadata/Avatar/ItemData.hpp>
#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Data/Character.hpp>
#include <OTwo/Data/Equipment.hpp>

#include <Genode/System/Module.hpp>
#include <Genode/IO/ResourceManager.hpp>

class ItemFactory : public Gx::Module
{
public:
    ItemFactory();
    ItemFactory(Gx::ResourceManager &sharedResources);

    const std::map<EquipmentType, Item*> GetDefaultItems(const Gender &gender);
    Item *GetItem(unsigned int id);

private:
    ItemData            *m_itemData;
    Gx::ResourceManager *m_resources;
};

#endif
