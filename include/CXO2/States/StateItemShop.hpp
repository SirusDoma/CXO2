#pragma once

#include <CXO2/States/State.hpp>
#include <CXO2/Avatar/Item.hpp>
#include <CXO2/Models/Equipment.hpp>
#include <CXO2/Models/Shop.hpp>
#include <CXO2/Metadata/Avatar/ItemMetadata.hpp>
#include <CXO2/Metadata/Avatar/SetInfoMetadata.hpp>
#include <CXO2/Contexts/CartContext.hpp>

#include <CXO2/Network/MessageEnvelope.hpp>
#include <CXO2/Network/Responses/PurchaseItemResponse.hpp>
#include <CXO2/Network/Responses/SellItemResponse.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/Tasks/Delay.hpp>
#include <Genode/Tasks/Scheduler.hpp>
#include <Genode/Tween/Move.hpp>
#include <CXO2/UI/Image.hpp>
#include <CXO2/UI/ScrollBar.hpp>

#include <optional>

namespace Cx
{
    class RadioButton;

    class ItemShopService;
    class ItemFactory;
    class SessionContext;
    class StateItemShop : public State
    {
    public:
        explicit StateItemShop(Gx::AudioMixer& mixer, ItemShopService& service, SessionContext& session, CartContext& cart, ItemFactory& items);
        void Initialize() override;

    private:
        void InitializeShopMaster(Cx::UiContainer* shopMaster, bool useSpeech = false);
        void SelectGender(Gender gender);

        void InvalidateShopMaster(bool moveIn = false);
        void InvalidateMyBag();
        void InvalidateCart();
        void InvalidateShopItemList(bool rebuildList = false);
        void InvalidateShopSetItemList(bool rebuildList = false);

        void OnPurchaseItemResponded(const MessageEnvelope<PurchaseItemResponse>& ev, const ItemMetadata& metadata);
        void OnSellItemResponded(const MessageEnvelope<SellItemResponse>& ev);

        void OnMyRoomButtonClicked(Control& sender, Control::Event& ev);
        void OnShopMasterClicked(Control& sender, Control::Event& ev);
        void OnDefaultButtonClicked(Control& sender, Control::Event& ev);

        void OnShopCategoryCheckChanged(Cx::RadioButton& sender, Control::Event& ev);
        void OnItemCategoryCheckChanged(Cx::RadioButton& sender, Control::Event& ev);

        void OnPlanetUpButtonClicked(Control& sender, Control::Event& ev);
        void OnPlanetDownButtonClicked(Control& sender, Control::Event& ev);
        void OnExtensionButtonClicked(Control& sender, Control::Event& ev);
        void OnPlanetExtensionButtonClicked(Control& sender, Control::Event& ev);
        void OnMaleButtonClicked(Control& sender, Control::Event& ev);
        void OnFemaleButtonClicked(Control& sender, Control::Event& ev);

        void OnShopScrollBarValueChanged(ScrollBar& sender, ScrollBar::ValueChangedEvent& ev);
        void OnShopScrollLeftButtonClicked(Control& sender, Control::Event& ev);
        void OnShopScrollRightButtonClicked(Control& sender, Control::Event& ev);
        void OnShopItemListScrolled(Control& sender, Control::Event& ev);

        void OnMyBagScrollBarValueChanged(ScrollBar& sender, ScrollBar::ValueChangedEvent& ev);
        void OnMyBagScrollLeftButtonClicked(Control& sender, Control::Event& ev);
        void OnMyBagScrollRightButtonClicked(Control& sender, Control::Event& ev);
        void OnMyBagListScrolled(Control& sender, Control::Event& ev);
        void OnMyBagSlotClicked(Control& sender, Control::Event& ev);
        void OnMyBagSlotDoubleClicked(Control& sender, Control::Event& ev);

        void OnSellButtonClicked(Control& sender, Control::Event& ev);
        void OnSellItemConfirmed(bool accepted);
        void OnBuyButtonClicked(Control& sender, Control::Event& ev);
        void OnGiftButtonClicked(Control& sender, Control::Event& ev);

        void OnCartPrevPageButtonClicked(Control& sender, Control::Event& ev);
        void OnCartNextPageButtonClicked(Control& sender, Control::Event& ev);
        void OnCartListScrolled(Control& sender, Control::Event& ev);
        void OnCartItemDeleteButtonClicked(Control& sender, Control::Event& ev);

        void OnMyBagButtonClicked(Control& sender, Control::Event& ev);
        void OnCartButtonClicked(Control& sender, Control::Event& ev);
        void OnBackButtonClicked(Control& sender, Control::Event& ev);

        void OnShopItemAddButtonClicked(Control& sender, Control::Event& ev);
        void OnShopItemPreviewButtonClicked(Control& sender, Control::Event& ev);
        void OnShopSetItemAddButtonClicked(Control& sender, Control::Event& ev);
        void OnShopSetItemPreviewButtonClicked(Control& sender, Control::Event& ev);
        void OnItemThumbnailFocusChanged(Control& sender, Control::Event& ev);

        Gx::AudioMixer& m_mixer;
        ItemShopService& m_service;
        SessionContext& m_session;
        CartContext& m_cart;
        ItemFactory& m_items;

        Planet m_shopPlanetCategory  = Planet::Unknown;
        ShopCategory m_shopCategory  = ShopCategory::Special;
        EquipmentType m_itemCategory = EquipmentType::Costume;
        Gender m_genderCategory      = Gender::Male;

        unsigned int m_myBagCurrentPage = 0;
        unsigned int m_cartCurrentPage = 0;
        unsigned int m_shopCurrentPage = 0;
        unsigned int m_shopMasterSpeechCounter = 0;

        Gx::Delay m_tooltipDelay;

        Item* m_myBagSelectedItem;
        Image* m_myBagSelectIndicator;
        std::optional<Gx::Move> m_shopMasterEffect;
        Gx::Scheduler m_extensionMenuEffect;
        std::vector<sf::Sound*> m_shopMasterSpeech;
        std::vector<Item> m_inventory;
        std::vector<ItemMetadata> m_shopItemList;
        std::vector<SetInfoMetadata> m_shopSetList;
        std::unordered_map<Planet, Cx::UiContainer*> m_shopMasters;
        std::unordered_map<unsigned int, std::vector<ItemMetadata>> m_shopSetItemList;
        std::unordered_map<unsigned int, std::unordered_map<Currency, unsigned int>> m_shopSetItemPrices;
        std::unordered_map<unsigned int, Item> m_thumbnails;

        std::unordered_map<ShopCategory, Cx::UiContainer*> m_shopCategoryContainerMap;
        std::unordered_map<ShopCategory, std::vector<EquipmentType>> m_itemCategoryMap;
        std::unordered_map<Cx::RadioButton*, ShopCategory> m_shopCategoryButtons;
        std::unordered_map<Cx::RadioButton*, std::pair<ShopCategory, std::size_t>> m_itemCategoryButtons;
        std::unordered_map<Control*, Planet> m_planetExtensionButtons;
        std::unordered_map<Control*, Item*> m_myBagSlotItems;
        std::unordered_map<Control*, unsigned int> m_myBagSlotQuantities;
        std::unordered_map<Control*, std::size_t> m_cartItemDeleteButtons;
        std::unordered_map<Control*, ItemMetadata> m_shopItemAddButtons;
        std::unordered_map<Control*, std::pair<ItemMetadata, unsigned int>> m_shopItemPreviewButtons;
        std::unordered_map<Control*, SetInfoMetadata> m_shopSetItemAddButtons;
        std::unordered_map<Control*, std::pair<SetInfoMetadata, unsigned int>> m_shopSetItemPreviewButtons;
        std::unordered_map<Control*, std::pair<sf::String, Cx::UiContainer*>> m_itemThumbnailTooltips;
    };
}
