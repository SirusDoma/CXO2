#ifndef O2JAM_ITEM_FACTORY_HPP
#define O2JAM_ITEM_FACTORY_HPP

#include <OTwo/Metadata/Character/ItemData.hpp>
#include <OTwo/Character/Item.hpp>

#include <Genode/System/Module.hpp>
#include <Genode/IO/ResourceManager.hpp>

class ItemFactory : public Gx::Module
{
public:
    ItemFactory();
    ItemFactory(Gx::ResourceManager &sharedResources);

    const std::map<Equipment::Type, Item*> GetDefaultItems(const Character::Gender &gender);
    Item *GetItem(unsigned int id);

private:
    ItemData            *m_itemData;
    Gx::ResourceManager *m_resources;
};

#endif
