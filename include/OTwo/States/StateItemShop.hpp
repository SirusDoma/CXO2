#pragma once

#include <OTwo/States/State.hpp>
#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Models/Equipment.hpp>
#include <OTwo/Metadata/Avatar/ItemMetadata.hpp>
#include <OTwo/Metadata/Avatar/SetInfoMetadata.hpp>
#include <OTwo/Contexts/CartContext.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/Tasks/Delay.hpp>
#include <Genode/Tasks/Scheduler.hpp>
#include <Genode/Tween/Move.hpp>
#include <Genode/UI/Image.hpp>

#include <optional>

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

    void OnExtensionButtonClicked();
    void OnItemSellClicked();
    void OnBuyButtonClicked();
    void OnGiftButtonClicked();

    void InvalidateShopMaster(bool moveIn = false);
    void InvalidateMyBag();
    void InvalidateCart();
    void InvalidateShopItemList(bool rebuildList = false);
    void InvalidateShopSetItemList(bool rebuildList = false);

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
};
