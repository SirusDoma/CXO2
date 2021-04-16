#ifndef O2JAM_ITEM_FACTORY_HPP
#define O2JAM_ITEM_FACTORY_HPP

#include <O2/Metadata/Character/ItemData.hpp>
#include <O2/Character/Item.hpp>

#include <Genode/IO/ResourceManager.hpp>

class ItemFactory
{
public:
    static void Initialize(Gx::ResourceManager &sharedResources);
    static const std::map<Equipment::Type, Item*> GetDefaultItems(const Character::Gender &gender);
    static Item *GetItem(unsigned int id);

private:
    inline static ItemData            *m_itemData;
    inline static Gx::ResourceManager *m_resources;
};

#endif
