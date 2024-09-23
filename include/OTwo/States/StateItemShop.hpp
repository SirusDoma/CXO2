#ifndef O2JAM_STATE_ITEM_SHOP_HPP
#define O2JAM_STATE_ITEM_SHOP_HPP

#include <mutex>
#include <OTwo/States/State.hpp>
#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Models/Equipment.hpp>
#include <OTwo/Metadata/Avatar/ItemMetadata.hpp>

#include <Genode/SceneGraph.hpp>
#include <Genode/Tasks/Delay.hpp>
#include <Genode/UI/Image.hpp>

class SessionContext;
class ItemFactory;
class StateItemShop : public State
{
public:
    explicit StateItemShop(Gx::Mixer& mixer, SessionContext& session, ItemFactory& items);
    void Initialize() override;

private:
    enum class ShopCategory
    {
        Special,
        Fashion,
        Accessory,
        Beauty,
        Instrument
    };

    void OnItemSellClicked();

    void InvalidateMyBag();
    void InvalidateShopItemList(bool rebuildList = false);

    Gx::Mixer& m_mixer;
    SessionContext& m_session;
    ItemFactory& m_items;

    Planet m_shopPlanetCategory  = Planet::Unknown;
    ShopCategory m_shopCategory  = ShopCategory::Special;
    EquipmentType m_itemCategory = EquipmentType::Costume;
    Gender m_genderCategory      = Gender::Male;

    unsigned int m_myBagCurrentPage = 0;
    unsigned int m_shopCurrentPage = 0;

    Gx::Delay m_tooltipDelay;

    Item* m_myBagSelectedItem;
    Gx::Image* m_myBagSelectIndicator;
    std::vector<Item*> m_inventory;
    std::vector<ItemMetadata> m_shopItemList;
    std::unordered_map<unsigned int, Gx::ResourcePtr<Item>> m_thumbnails;
    std::unordered_map<unsigned int, std::mutex> m_mutexes;
};


#endif
