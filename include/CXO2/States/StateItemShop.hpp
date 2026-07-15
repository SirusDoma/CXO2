#pragma once

#include <CXO2/States/State.hpp>
#include <CXO2/Avatar/Item.hpp>
#include <CXO2/Models/Equipment.hpp>
#include <CXO2/Metadata/Avatar/ItemMetadata.hpp>
#include <CXO2/Metadata/Avatar/SetInfoMetadata.hpp>
#include <CXO2/Contexts/CartContext.hpp>

#include <CXO2/Messages/MessageEnvelope.hpp>
#include <CXO2/Messages/Responses/PurchaseItemResponse.hpp>
#include <CXO2/Messages/Responses/SellItemResponse.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/Tasks/Delay.hpp>
#include <Genode/Tasks/Scheduler.hpp>
#include <Genode/Tween/Move.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/ScrollBar.hpp>

#include <optional>

namespace Gx
{
    class RadioButton;
}

namespace Cx
{
    class ItemShopService;
    class ItemFactory;
    class SessionContext;
    class StateItemShop : public State
    {
    public:
        explicit StateItemShop(Gx::AudioMixer& mixer, ItemShopService& service, SessionContext& session, CartContext& cart, ItemFactory& items);
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

        void InitializeShopMaster(Gx::UiContainer* shopMaster, bool useSpeech = false);
        void SelectGender(Gender gender);

        void InvalidateShopMaster(bool moveIn = false);
        void InvalidateMyBag();
        void InvalidateCart();
        void InvalidateShopItemList(bool rebuildList = false);
        void InvalidateShopSetItemList(bool rebuildList = false);

        void OnPurchaseItemResponded(const MessageEnvelope<PurchaseItemResponse>& ev, const ItemMetadata& metadata);
        void OnSellItemResponded(const MessageEnvelope<SellItemResponse>& ev);

        void OnMyRoomButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnShopMasterClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnDefaultButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnShopCategoryCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev);
        void OnItemCategoryCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev);

        void OnPlanetUpButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnPlanetDownButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnExtensionButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnPlanetExtensionButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMaleButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnFemaleButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnShopScrollBarValueChanged(Gx::ScrollBar& sender, Gx::ScrollBar::ValueChangedEvent& ev);
        void OnShopScrollLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnShopScrollRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnShopItemListScrolled(Gx::Control& sender, Gx::Control::Event& ev);

        void OnMyBagScrollBarValueChanged(Gx::ScrollBar& sender, Gx::ScrollBar::ValueChangedEvent& ev);
        void OnMyBagScrollLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMyBagScrollRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMyBagListScrolled(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMyBagSlotClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMyBagSlotDoubleClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnSellButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnSellItemConfirmed(bool accepted);
        void OnBuyButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnGiftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnCartPrevPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCartNextPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCartListScrolled(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCartItemDeleteButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnMyBagButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnCartButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnBackButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnShopItemAddButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnShopItemPreviewButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnShopSetItemAddButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnShopSetItemPreviewButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnItemThumbnailFocusChanged(Gx::Control& sender, Gx::Control::Event& ev);

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
        Gx::Image* m_myBagSelectIndicator;
        std::optional<Gx::Move> m_shopMasterEffect;
        Gx::Scheduler m_extensionMenuEffect;
        std::vector<sf::Sound*> m_shopMasterSpeech;
        std::vector<Item> m_inventory;
        std::vector<ItemMetadata> m_shopItemList;
        std::vector<SetInfoMetadata> m_shopSetList;
        std::unordered_map<Planet, Gx::UiContainer*> m_shopMasters;
        std::unordered_map<unsigned int, std::vector<ItemMetadata>> m_shopSetItemList;
        std::unordered_map<unsigned int, std::unordered_map<Currency, unsigned int>> m_shopSetItemPrices;
        std::unordered_map<unsigned int, Item> m_thumbnails;

        std::unordered_map<ShopCategory, Gx::UiContainer*> m_shopCategoryContainerMap;
        std::unordered_map<ShopCategory, std::vector<EquipmentType>> m_itemCategoryMap;
        std::unordered_map<Gx::RadioButton*, ShopCategory> m_shopCategoryButtons;
        std::unordered_map<Gx::RadioButton*, std::pair<ShopCategory, std::size_t>> m_itemCategoryButtons;
        std::unordered_map<Gx::Control*, Planet> m_planetExtensionButtons;
        std::unordered_map<Gx::Control*, Item*> m_myBagSlotItems;
        std::unordered_map<Gx::Control*, unsigned int> m_myBagSlotQuantities;
        std::unordered_map<Gx::Control*, std::size_t> m_cartItemDeleteButtons;
        std::unordered_map<Gx::Control*, ItemMetadata> m_shopItemAddButtons;
        std::unordered_map<Gx::Control*, std::pair<ItemMetadata, unsigned int>> m_shopItemPreviewButtons;
        std::unordered_map<Gx::Control*, SetInfoMetadata> m_shopSetItemAddButtons;
        std::unordered_map<Gx::Control*, std::pair<SetInfoMetadata, unsigned int>> m_shopSetItemPreviewButtons;
        std::unordered_map<Gx::Control*, std::pair<sf::String, Gx::UiContainer*>> m_itemThumbnailTooltips;
    };
}
