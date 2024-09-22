#ifndef O2JAM_STATE_ITEM_SHOP_HPP
#define O2JAM_STATE_ITEM_SHOP_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Models/Equipment.hpp>

#include <Genode/SceneGraph.hpp>
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

    Gx::Mixer& m_mixer;
    SessionContext& m_session;
    ItemFactory& m_items;

    ShopCategory m_shopCategory       = ShopCategory::Special;
    EquipmentType m_itemCategory = EquipmentType::Costume;
    Gender m_genderCategory           = Gender::Male;

    unsigned int m_myBagCurrentPage = 0;
    unsigned int m_myBagMaxPage     = 0;

    Item* m_myBagSelectedItem;
    Gx::Image* m_myBagSelectIndicator;
    std::vector<Item*> m_inventory;
};


#endif
