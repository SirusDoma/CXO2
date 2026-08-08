#include <CXO2/States/StateItemShop.hpp>

#include <CXO2/Events/ItemShopEvents.hpp>

#include <CXO2/States/StatePlanet.hpp>
#include <CXO2/States/StateRoom.hpp>
#include <CXO2/States/StateMyRoom.hpp>
#include <CXO2/States/StatePayment.hpp>

#include <CXO2/O2.hpp>
#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/CartContext.hpp>

#include <CXO2/Avatar/ItemFactory.hpp>

#include <CXO2/Services/ItemShopService.hpp>
#include <CXO2/Network/Requests/PurchaseItemRequest.hpp>
#include <CXO2/Network/Requests/SellItemRequest.hpp>

#include <CXO2/Network/Responses/PurchaseItemResponse.hpp>
#include <CXO2/Network/Responses/SellItemResponse.hpp>

#include <CXO2/Constants/Identifiers/Sound.hpp>
#include <CXO2/Constants/Identifiers/ItemShop.hpp>
#include <CXO2/Constants/Messages/ItemShop.hpp>
#include <CXO2/Utilities/StringFormatter.hpp>

#include <Genode/Network/Exception.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/ScrollBar.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/RadioButton.hpp>

#include <magic_enum/magic_enum.hpp>
#include <fmt/format.h>

namespace Cx
{
    using namespace Constants::Identifiers;

    StateItemShop::StateItemShop(Gx::AudioMixer& mixer, ItemShopService& service, SessionContext& session, CartContext& cart, ItemFactory& items) :
        m_mixer(mixer),
        m_service(service),
        m_session(session),
        m_cart(cart),
        m_items(items),
        m_myBagSelectedItem(),
        m_myBagSelectIndicator(),
        m_shopMasterEffect()
    {
    }

    void StateItemShop::Initialize()
    {
        if (!State::Initialize(StateEventArgs{GetName()}))
            return;

        const auto bgm        = Instantiate<sf::Music>(Sound::BGM::BG_ITEM_SHOP);
        const auto sfxWelcome = Instantiate<sf::Sound>(Sound::Speech::NPC_1);
        const auto sfxAccept  = Instantiate<sf::Sound>(Sound::Effects::EF_02);
        const auto sfxCancel  = Instantiate<sf::Sound>(Sound::Effects::EF_03);
        const auto sfxPrev    = Instantiate<sf::Sound>(Sound::Effects::EF_19_1);
        const auto sfxNext    = Instantiate<sf::Sound>(Sound::Effects::EF_19_2);
        const auto sfxMenu    = Instantiate<sf::Sound>(Sound::Effects::EF_11);
        const auto sfxGender  = Instantiate<sf::Sound>(Sound::Effects::EF_15);
        const auto sfxPlanet  = Instantiate<sf::Sound>(Sound::Effects::EF_24_);

        for (auto id : { Sound::Speech::NPC_5, Sound::Speech::NPC_6, Sound::Speech::NPC_7 })
            m_shopMasterSpeech.push_back(Instantiate<sf::Sound>(id));

        m_genderCategory = m_session.GetGender();
        m_shopCategory   = ShopCategory::Special;
        m_itemCategory   = EquipmentType::Costume;

        const auto avatar = Instantiate<Avatar>(Resource::ItemShop::IDC_AVATAR);
        avatar->SetGender(m_session.GetGender());
        for (auto& [_, item] : m_items.GetDefaultItems(m_session.GetGender()))
            avatar->SetDefaultItem(std::move(item));

        for (const auto id : m_session.GetEquippedItemIDs())
            avatar->Equip(m_items.Create(id));

        const auto nicknameText = Instantiate<Gx::Label>(Resource::ItemShop::IDC_TEXT_NICKNAME);
        nicknameText->SetString(fmt::format(Constants::Messages::ItemShop::NICKNAME, m_session.GetLevel(), m_session.GetName()));

        const auto currentGem = Instantiate<Gx::BitmapNumber>(Resource::ItemShop::IDC_NUMBER_GEM);
        currentGem->SetValue(m_session.GetWallet().Gem);

        const auto currentCash = Instantiate<Gx::BitmapNumber>(Resource::ItemShop::IDC_NUMBER_CASH);
        currentCash->SetValue(m_session.GetWallet().Cash);

        const auto myRoomButton = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_MY_ROOM);
        myRoomButton->SetClickCallback([this] (auto& sender, auto& ev) { OnMyRoomButtonClicked(sender, ev); });

        const auto shopMasterContainer = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_SHOP_MASTER);
        const auto shopMasterMain = shopMasterContainer->FindChild<Gx::UiContainer>(Resource::ItemShop::ShopMaster::IDC_IMAGE_SHOP_MASTER_MAIN);
        InitializeShopMaster(shopMasterMain, true);

        const auto shopMasterO2P = shopMasterContainer->FindChild<Gx::UiContainer>(Resource::ItemShop::ShopMaster::IDC_IMAGE_SHOP_MASTER_O2P);
        shopMasterO2P->SetVisible(false);
        InitializeShopMaster(shopMasterO2P);

        const auto shopMasterAqua = shopMasterContainer->FindChild<Gx::UiContainer>(Resource::ItemShop::ShopMaster::IDC_IMAGE_SHOP_MASTER_AQUA);
        shopMasterAqua->SetVisible(false);
        InitializeShopMaster(shopMasterAqua);

        const auto shopMasterGraffiti = shopMasterContainer->FindChild<Gx::UiContainer>(Resource::ItemShop::ShopMaster::IDC_IMAGE_SHOP_MASTER_GRAFFITI);
        shopMasterGraffiti->SetVisible(false);
        InitializeShopMaster(shopMasterGraffiti);

        const auto shopMasterEvent = shopMasterContainer->FindChild<Gx::UiContainer>(Resource::ItemShop::ShopMaster::IDC_IMAGE_SHOP_MASTER_EVENT);
        shopMasterEvent->SetVisible(false);
        InitializeShopMaster(shopMasterEvent);

        m_shopMasters = {
            { Planet::Unknown,    shopMasterMain     },
            { Planet::O2Planet,   shopMasterO2P      },
            { Planet::Aqua,       shopMasterAqua     },
            { Planet::Graffiti,   shopMasterGraffiti },
            { Planet::Crush,      shopMasterGraffiti },
            { Planet::Wonderland, shopMasterAqua     },
            { Planet::Meganut,    shopMasterO2P      },
            { Planet::Draconic,   shopMasterO2P      },
            { Planet::Event,      shopMasterEvent    }
        };

        const auto tooltip = Instantiate<Gx::Image>(Resource::ItemShop::IDC_IMAGE_TOOLTIP);
        tooltip->SetVisible(false);

        const auto tooltipMessage = tooltip->FindChild<Gx::Label>(Resource::ItemShop::IDC_TEXT_MESSAGE);
        tooltipMessage->SetLocalBounds(tooltip->GetLocalBounds());

        const auto defaultButton = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_DEFAULT);
        defaultButton->SetClickCallback([this] (auto& sender, auto& ev) { OnDefaultButtonClicked(sender, ev); });

        const auto categoryButtonsContainer = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_CATEGORY_BUTTONS);
        const std::unordered_map<ShopCategory, Gx::RadioButton*> shopCategoryButtonMap =
        {
            { ShopCategory::Special,    categoryButtonsContainer->FindChild<Gx::RadioButton>(Resource::ItemShop::IDC_BUTTON_SPECIAL) },
            { ShopCategory::Fashion,    categoryButtonsContainer->FindChild<Gx::RadioButton>(Resource::ItemShop::IDC_BUTTON_FASHION) },
            { ShopCategory::Accessory,  categoryButtonsContainer->FindChild<Gx::RadioButton>(Resource::ItemShop::IDC_BUTTON_ACCESSORY) },
            { ShopCategory::Beauty,     categoryButtonsContainer->FindChild<Gx::RadioButton>(Resource::ItemShop::IDC_BUTTON_BEAUTY) },
            { ShopCategory::Instrument, categoryButtonsContainer->FindChild<Gx::RadioButton>(Resource::ItemShop::IDC_BUTTON_INSTRUMENT) },
       };

        m_shopCategoryContainerMap =
        {
            { ShopCategory::Special,    Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_SPECIAL_CATEGORY) },
            { ShopCategory::Fashion,    Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_FASHION_CATEGORY) },
            { ShopCategory::Accessory,  Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_ACCESSORY_CATEGORY) },
            { ShopCategory::Beauty,     Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_BEAUTY_CATEGORY) },
            { ShopCategory::Instrument, Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_INSTRUMENT_CATEGORY) },
        };

        m_itemCategoryMap = []
        {
            if (O2::InInteropMode(InteropMode::Avatar))
            {
                return std::unordered_map<ShopCategory, std::vector<EquipmentType>>
                {
                    { ShopCategory::Fashion,    { EquipmentType::Top, EquipmentType::Pants, EquipmentType::Shoes } },
                    { ShopCategory::Accessory,  { EquipmentType::Accessories, EquipmentType::Earrings, EquipmentType::Necklace, EquipmentType::ClothesAccessories, EquipmentType::Glasses, EquipmentType::Gloves } },
                    { ShopCategory::Beauty,     { EquipmentType::Hair, EquipmentType::HairAccessories, EquipmentType::Face } },
                    { ShopCategory::Instrument, { EquipmentType::Guitar, EquipmentType::Bass, EquipmentType::Keyboard, EquipmentType::Drum } },
                };
            }
            else
            {
                return std::unordered_map<ShopCategory, std::vector<EquipmentType>>
                {
                    { ShopCategory::Special,    { EquipmentType::Costume, EquipmentType::Wings, EquipmentType::Pet, EquipmentType::AttributiveItem } },
                    { ShopCategory::Fashion,    { EquipmentType::Top, EquipmentType::Pants, EquipmentType::ClothesAccessories, EquipmentType::Shoes } },
                    { ShopCategory::Accessory,  { EquipmentType::Accessories, EquipmentType::Earrings, EquipmentType::Necklace, EquipmentType::Glasses, EquipmentType::Gloves } },
                    { ShopCategory::Beauty,     { EquipmentType::Hair, EquipmentType::HairAccessories, EquipmentType::Face } },
                    { ShopCategory::Instrument, { EquipmentType::Guitar, EquipmentType::Bass, EquipmentType::Keyboard, EquipmentType::Drum, EquipmentType::InstrumentAccessories } },
                };
            }

        }();

        if (const auto it = shopCategoryButtonMap.find(ShopCategory::Special); it != shopCategoryButtonMap.end())
        {
            m_shopCategory = !it->second || !it->second->GetTexture() || it->second->GetLocalBounds() == sf::FloatRect() ? ShopCategory::Fashion : m_shopCategory;
            if (m_shopCategory == ShopCategory::Fashion)
                m_itemCategory = EquipmentType::Top;
        }
        for (auto [category, button] : shopCategoryButtonMap)
        {
            m_shopCategoryContainerMap.at(category)->SetEnabled(category == m_shopCategory);
            m_shopCategoryContainerMap.at(category)->SetVisible(category == m_shopCategory);

            button->SetCheckedState(category == m_shopCategory);
            m_shopCategoryButtons[button] = category;
            button->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnShopCategoryCheckChanged(sender, ev); });

            auto children = m_shopCategoryContainerMap.at(category)->GetChildren();
            for (std::size_t i = 0; i < children.size(); i++)
            {
                if (const auto radio = dynamic_cast<Gx::RadioButton*>(children[i]))
                {
                    radio->SetCheckedState(category == m_shopCategory && i == 0);
                    m_itemCategoryButtons[radio] = { category, i };
                    radio->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnItemCategoryCheckChanged(sender, ev); });
                }
            }
        }

        const auto planetPrevButton = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_PLANET_UP);
        const auto planetNextButton = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_PLANET_DOWN);

        planetPrevButton->SetClickCallback([this] (auto& sender, auto& ev) { OnPlanetUpButtonClicked(sender, ev); });
        planetNextButton->SetClickCallback([this] (auto& sender, auto& ev) { OnPlanetDownButtonClicked(sender, ev); });

        const auto extButton = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_EXT_MENU);
        extButton->SetClickCallback([this] (auto& sender, auto& ev) { OnExtensionButtonClicked(sender, ev); });

        const auto planetExt = Instantiate<Gx::Image>(Resource::ItemShop::IDC_IMAGE_EXT_MENU);
        planetExt->SetTexCoords({});
        planetExt->SetEnabled(false);
        const std::unordered_map<Planet, Gx::Button*> shopPlanetExtMap =
        {
            { Planet::Unknown,    planetExt->FindChild<Gx::Button>(Resource::ItemShop::ExtensionMenu::IDC_BUTTON_EXT_ALL) },
            { Planet::O2Planet,   planetExt->FindChild<Gx::Button>(Resource::ItemShop::ExtensionMenu::IDC_BUTTON_EXT_NORMAL) },
            { Planet::Aqua,       planetExt->FindChild<Gx::Button>(Resource::ItemShop::ExtensionMenu::IDC_BUTTON_EXT_AQUA) },
            { Planet::Eliten,     planetExt->FindChild<Gx::Button>(Resource::ItemShop::ExtensionMenu::IDC_BUTTON_EXT_ELITEN) },
            { Planet::Graffiti,   planetExt->FindChild<Gx::Button>(Resource::ItemShop::ExtensionMenu::IDC_BUTTON_EXT_GRAFFITI) },
            { Planet::Bikini,     planetExt->FindChild<Gx::Button>(Resource::ItemShop::ExtensionMenu::IDC_BUTTON_EXT_BIKINI) },
            { Planet::Crush,      planetExt->FindChild<Gx::Button>(Resource::ItemShop::ExtensionMenu::IDC_BUTTON_EXT_CRUSH) },
            { Planet::Wonderland, planetExt->FindChild<Gx::Button>(Resource::ItemShop::ExtensionMenu::IDC_BUTTON_EXT_WONDERLAND) },
            { Planet::Meganut,    planetExt->FindChild<Gx::Button>(Resource::ItemShop::ExtensionMenu::IDC_BUTTON_EXT_MEGANUT) },
            { Planet::Crystal,    planetExt->FindChild<Gx::Button>(Resource::ItemShop::ExtensionMenu::IDC_BUTTON_EXT_CRYSTAL) },
            { Planet::Draconic,   planetExt->FindChild<Gx::Button>(Resource::ItemShop::ExtensionMenu::IDC_BUTTON_EXT_DRACONIC) },
            { Planet::Event,      planetExt->FindChild<Gx::Button>(Resource::ItemShop::ExtensionMenu::IDC_BUTTON_EXT_EVENT) },
       };

        for (auto [planet, button] : shopPlanetExtMap)
        {
            m_planetExtensionButtons[button] = planet;
            button->SetClickCallback([this] (auto& sender, auto& ev) { OnPlanetExtensionButtonClicked(sender, ev); });
        }

        const auto maleButton   = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_MALE);
        const auto femaleButton = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_FEMALE);

        maleButton->SetEnabled(m_genderCategory == Gender::Male);
        maleButton->SetVisible(m_genderCategory == Gender::Male);
        femaleButton->SetEnabled(m_genderCategory == Gender::Female);
        femaleButton->SetVisible(m_genderCategory == Gender::Female);

        maleButton->SetClickCallback([this] (auto& sender, auto& ev) { OnMaleButtonClicked(sender, ev); });
        femaleButton->SetClickCallback([this] (auto& sender, auto& ev) { OnFemaleButtonClicked(sender, ev); });

        m_shopCurrentPage = 0;
        const auto itemScrollControls = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_ITEM_SCROLL_CONTROLS);
        const auto shopScrollBar      = itemScrollControls->FindChild<Gx::ScrollBar>(Resource::ItemShop::IDC_SCROLL_ITEM);
        const auto itemList           = Instantiate<Gx::List>(Resource::ItemShop::IDC_LIST_ITEM);

        // TODO: Detect vertical count?
        constexpr int verticalCount = 2; //itemList->GetVerticalCount()
        shopScrollBar->SetMaximumValue(m_shopItemList.size() < itemList->GetChildrenCount() ? 0 : static_cast<int>(std::ceil(static_cast<float>(m_shopItemList.size() - itemList->GetChildrenCount()) / verticalCount)));
        shopScrollBar->SetValueChangedCallback([this] (auto& sender, auto& ev) { OnShopScrollBarValueChanged(sender, ev); });

        const auto shopScrollLeft = itemScrollControls->FindChild<Gx::Button>(Resource::ItemShop::IDC_BUTTON_ITEM_SCROLL_LEFT);
        shopScrollLeft->SetClickCallback([this] (auto& sender, auto& ev) { OnShopScrollLeftButtonClicked(sender, ev); });

        const auto shopScrollRight = itemScrollControls->FindChild<Gx::Button>(Resource::ItemShop::IDC_BUTTON_ITEM_SCROLL_RIGHT);
        shopScrollRight->SetClickCallback([this] (auto& sender, auto& ev) { OnShopScrollRightButtonClicked(sender, ev); });

        const auto shopItemList = Instantiate<Gx::List>(Resource::ItemShop::IDC_LIST_ITEM);
        shopItemList->SetScrollWheelCallback([this] (auto& sender, auto& ev) { OnShopItemListScrolled(sender, ev); });

        const auto setItemContainer = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_SET_ITEM);
        const auto setItemList      = setItemContainer->FindChild<Gx::List>(Resource::ItemShop::IDC_LIST_SET_ITEM);
        setItemList->SetScrollWheelCallback([this] (auto& sender, auto& ev) { OnShopItemListScrolled(sender, ev); });

        const auto myBagContainer = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_MYBAG);
        m_myBagSelectIndicator = myBagContainer->FindChild<Gx::Image>(Resource::ItemShop::MyBag::IDC_IMAGE_MYBAG_SELECT);
        m_myBagSelectIndicator->SetVisible(false);

        const auto bagList = myBagContainer->FindChild<Gx::List>(Resource::ItemShop::MyBag::IDC_LIST_BAG);
        const auto bagSlots = bagList->GetChildren();

        m_myBagCurrentPage = 0;
        const auto bagScrollControls = myBagContainer->FindChild<Gx::UiContainer>(Resource::ItemShop::MyBag::IDC_CONTAINER_MYBAG_SCROLL_CONTROLS);
        const auto bagScrollBar = bagScrollControls->FindChild<Gx::ScrollBar>(Resource::ItemShop::MyBag::IDC_SCROLL_MYBAG);
        bagScrollBar->SetMaximumValue(m_inventory.size() < bagSlots.size() ? 0 : static_cast<int>(std::ceil(static_cast<float>(m_inventory.size() - bagSlots.size()) / bagList->GetVerticalCount())));
        bagScrollBar->SetValueChangedCallback([this] (auto& sender, auto& ev) { OnMyBagScrollBarValueChanged(sender, ev); });

        const auto bagScrollLeftButton = bagScrollControls->FindChild<Gx::Button>(Resource::ItemShop::MyBag::IDC_BUTTON_MYBAG_SCROLL_LEFT);
        bagScrollLeftButton->SetClickCallback([this] (auto& sender, auto& ev) { OnMyBagScrollLeftButtonClicked(sender, ev); });

        const auto bagScrollRightButton = bagScrollControls->FindChild<Gx::Button>(Resource::ItemShop::MyBag::IDC_BUTTON_MYBAG_SCROLL_RIGHT);
        bagScrollRightButton->SetClickCallback([this] (auto& sender, auto& ev) { OnMyBagScrollRightButtonClicked(sender, ev); });

        bagList->SetScrollWheelCallback([this] (auto& sender, auto& ev) { OnMyBagListScrolled(sender, ev); });

        const auto sellButton = myBagContainer->FindChild<Gx::Button>(Resource::ItemShop::MyBag::IDC_BUTTON_SELL);
        sellButton->SetClickCallback([this] (auto& sender, auto& ev) { OnSellButtonClicked(sender, ev); });

        m_cartCurrentPage = 0;
        const auto cartContainer = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_CART);

        const auto buyButton = cartContainer->FindChild<Gx::Button>(Resource::ItemShop::Cart::IDC_BUTTON_BUY);
        buyButton->SetClickCallback([this] (auto& sender, auto& ev) { OnBuyButtonClicked(sender, ev); });

        const auto giftButton = cartContainer->FindChild<Gx::Button>(Resource::ItemShop::Cart::IDC_BUTTON_GIFT);
        giftButton->SetClickCallback([this] (auto& sender, auto& ev) { OnGiftButtonClicked(sender, ev); });

        const auto cartList           = cartContainer->FindChild<Gx::List>(Resource::ItemShop::Cart::IDC_LIST_CART);
        const auto cartPrevPageButton = cartContainer->FindChild<Gx::Button>(Resource::ItemShop::Cart::IDC_BUTTON_LEFT);
        const auto cartNextPageButton = cartContainer->FindChild<Gx::Button>(Resource::ItemShop::Cart::IDC_BUTTON_RIGHT);

        cartPrevPageButton->SetClickCallback([this] (auto& sender, auto& ev) { OnCartPrevPageButtonClicked(sender, ev); });
        cartNextPageButton->SetClickCallback([this] (auto& sender, auto& ev) { OnCartNextPageButtonClicked(sender, ev); });

        cartList->SetScrollWheelCallback([this] (auto& sender, auto& ev) { OnCartListScrolled(sender, ev); });

        const auto myBagButton = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_MYBAG);
        const auto cartButton  = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_CART);

        myBagButton->SetClickCallback([this] (auto& sender, auto& ev) { OnMyBagButtonClicked(sender, ev); });
        cartButton->SetClickCallback([this] (auto& sender, auto& ev) { OnCartButtonClicked(sender, ev); });

        const auto backButton = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_BACK);
        backButton->SetClickCallback([this] (auto& sender, auto& ev) { OnBackButtonClicked(sender, ev); });

        myBagButton->PerformClick();
        InvalidateMyBag();
        InvalidateCart();
        InvalidateShopItemList(true);

        bgm->setLooping(true);
        m_mixer.Play(*bgm, Sound::Channel::BGM);
        m_mixer.Play(*sfxWelcome, Sound::Channel::SFX);
    }

    void StateItemShop::InitializeShopMaster(Gx::UiContainer* shopMaster, bool useSpeech)
    {
        if (!shopMaster)
            return;

        if (const auto speech = shopMaster->FindChild<Gx::Animation>(Resource::ItemShop::ShopMaster::IDC_ANIMATION_SPEECH))
        {
            speech->SetAnimationCallback([=] (Gx::Animation& animation)
            {
                animation.SetVisible(
                    animation.GetState() == Gx::Animation::AnimationState::Playing ||
                    animation.GetState() == Gx::Animation::AnimationState::Initial
                );
            });

            speech->Stop();
        }

        if (useSpeech)
        {
            const auto main = shopMaster->FindChild<Gx::Image>(Resource::ItemShop::ShopMaster::IDC_IMAGE_MAIN);
            main->SetClickCallback([this] (auto& sender, auto& ev) { OnShopMasterClicked(sender, ev); });
        }
    }

    void StateItemShop::OnPurchaseItemResponded(const MessageEnvelope<PurchaseItemResponse>& ev, const ItemMetadata& metadata)
    {
        try
        {
            const auto& response = ev.Open();
            if (Dispatch(ItemShopEvents::OnPurchaseItemResponded, ItemShopPurchaseResponseEventArgs{response, metadata}))
                return;

            if (response.ResultCode != PurchaseItemResult::Success)
            {
                if (response.ResultCode == PurchaseItemResult::InsufficientMoney)
                    ShowDialog(Constants::Messages::ItemShop::PurchaseResponse::NOT_ENOUGH_MONEY, DialogStyle::Information);
                else if (response.ResultCode == PurchaseItemResult::InventoryFull)
                    ShowDialog(Constants::Messages::ItemShop::PurchaseResponse::NO_VACANT_SLOT, DialogStyle::Information);

                return;
            }

            const auto& inventory = m_session.GetInventory();
            if (inventory[response.SlotID] != 0)
            {
                ShowDialog("Invalid inventory slot.", DialogStyle::Information);
                return;
            }

            m_session.SetInventoryItem(response.SlotID, metadata.ID);

            m_session.SetWallet({ response.Gem, response.Cash });

            // m_session.Save();
            m_inventory.clear();
            InvalidateMyBag();

            const auto bagButton = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_MYBAG);
            bagButton->PerformClick();
        }
        catch (const Gx::Exception& ex)
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, [=] (bool)
            {
                GetDirector().Present<StatePlanet>();
            });
        }
    }

    void StateItemShop::OnSellItemResponded(const MessageEnvelope<SellItemResponse>& ev)
    {
        const auto sfxAccept = Instantiate<sf::Sound>(Sound::Effects::EF_02);

        try
        {
            const auto& response = ev.Open();
            if (Dispatch(ItemShopEvents::OnSellItemResponded, ItemShopSellResponseEventArgs{response}))
                return;

            if (response.Result == SellItemResult::Failed)
            {
                ShowDialog(Constants::Messages::ItemShop::STANDARD_FACE_LOCKED, DialogStyle::Information);
                return;
            }

            m_session.SetInventoryItem(response.SlotID, 0);
            m_inventory[response.SlotID] = Item{};

            m_session.SetWallet({ response.Gem, response.Cash });

            m_myBagSelectedItem = nullptr;
            m_mixer.Play(*sfxAccept, Sound::Channel::SFX);

            // m_session.Save();
            InvalidateMyBag();
        }
        catch (const Gx::Exception& ex)
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information);
        }
    }

    void StateItemShop::OnExtensionButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        if (m_extensionMenuEffect.GetState() == Gx::TaskState::Running)
            return;

        if (m_extensionMenuEffect.GetState() != Gx::TaskState::Idle)
            Stop(m_extensionMenuEffect);

        const auto extMenu          = Instantiate<Gx::Image>(Resource::ItemShop::IDC_IMAGE_EXT_MENU);
        const auto extDefaultFrame  = extMenu->GetFrame(0);
        const auto itemList         = Instantiate<Gx::List>(Resource::ItemShop::IDC_LIST_ITEM);
        const auto setItemContainer = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_SET_ITEM);
        const auto setItemList      = setItemContainer->FindChild<Gx::List>(Resource::ItemShop::IDC_LIST_SET_ITEM);
        const auto scrollControls   = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_ITEM_SCROLL_CONTROLS);
        const auto shopScrollBar    = scrollControls->FindChild<Gx::ScrollBar>(Resource::ItemShop::IDC_SCROLL_ITEM);

        bool open = true;
        const bool invalidate = shopScrollBar->GetValue() == 0;
        if (extMenu->IsEnabled())
        {
            open = false;
            extMenu->SetEnabled(false);
            extMenu->SetTexCoords(extDefaultFrame->TexCoords);

            itemList->SetHorizontalRepeat(itemList->GetHorizontalCount() + 1, itemList->GetHorizontalSpacing());
            itemList->SetPosition(itemList->GetPosition().x - itemList->GetHorizontalSpacing(), itemList->GetPosition().y);

            setItemList->SetHorizontalRepeat(setItemList->GetHorizontalCount() + 1, setItemList->GetHorizontalSpacing());
            setItemList->SetPosition(setItemList->GetPosition().x - setItemList->GetHorizontalSpacing(), setItemList->GetPosition().y);

            shopScrollBar->SetValue(0);
            if (invalidate)
                InvalidateShopItemList();
        }
        else
        {
            extMenu->SetTexCoords({});

            itemList->SetHorizontalRepeat(itemList->GetHorizontalCount() - 1, itemList->GetHorizontalSpacing());
            itemList->SetPosition(itemList->GetPosition().x + itemList->GetHorizontalSpacing(), itemList->GetPosition().y);

            setItemList->SetHorizontalRepeat(setItemList->GetHorizontalCount() - 1, setItemList->GetHorizontalSpacing());
            setItemList->SetPosition(setItemList->GetPosition().x + setItemList->GetHorizontalSpacing(), setItemList->GetPosition().y);

            shopScrollBar->SetValue(0);
            if (invalidate)
                InvalidateShopItemList();
        }

        m_extensionMenuEffect = Gx::Scheduler(
            sf::seconds(1.f),
            sf::seconds(1.f / 60.f),
            [=] (const auto& task, auto _)
            {
                if (task.GetState() == Gx::TaskState::Completed)
                {
                    if (open)
                    {
                        extMenu->SetEnabled(true);
                        extMenu->SetTexCoords(extDefaultFrame->TexCoords);
                    }
                    else
                    {
                        extMenu->SetEnabled(false);
                        extMenu->SetTexCoords({});
                    }
                }
                else
                {
                    const auto& texCoords = extDefaultFrame->TexCoords;
                    if (open)
                    {
                        extMenu->SetTexCoords({
                            { static_cast<int>(texCoords.size.x - (texCoords.size.x * (task.GetElapsed().asSeconds() / task.GetDuration().asSeconds()))), texCoords.position.y },
                            { static_cast<int>(texCoords.size.x * (task.GetElapsed().asSeconds() / task.GetDuration().asSeconds())), texCoords.size.y }
                        });
                    }
                    else
                    {
                        extMenu->SetTexCoords({
                            { static_cast<int>((texCoords.size.x * (task.GetElapsed().asSeconds() / task.GetDuration().asSeconds()))), texCoords.position.y },
                            { static_cast<int>(texCoords.size.x * (1.f - (task.GetElapsed().asSeconds() / task.GetDuration().asSeconds()))), texCoords.size.y }
                        });
                    }
                }
            }
        );

        Run(m_extensionMenuEffect);
    }

    void StateItemShop::OnBuyButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        if (m_cart.GetItems().size() == 0)
        {
            ShowDialog("Shopping bag is empty", DialogStyle::Information);
            return;
        }

        ShowDialog("Would you like to move\nto the transaction window?", DialogStyle::YesNo, [=] (const bool answer)
        {
            if (answer)
            {
                m_cart.SetCheckoutType(CartContext::CheckoutType::Item);
                m_mixer.Play(*Instantiate<sf::Sound>(Sound::Effects::EF_02), Sound::Channel::SFX);
                GetDirector().Present<StatePayment>();
            }
            else
                m_mixer.Play(*Instantiate<sf::Sound>(Sound::Effects::EF_03), Sound::Channel::SFX);
        });
    }

    void StateItemShop::OnGiftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        if (m_cart.GetItems().size() == 0)
        {
            ShowDialog("Shopping bag is empty", DialogStyle::Information);
            return;
        }

        ShowDialog("Gift is currently not available", DialogStyle::Information);
    }

    void StateItemShop::OnSellButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        if (!m_myBagSelectedItem)
        {
            ShowDialog(Constants::Messages::ItemShop::NOTHING_SELECTED, DialogStyle::Information);
            return;
        }

        auto currency = Currency::Gem;
        auto price    = 0;

        for (auto cur : { Currency::Gem, Currency::Cash })
        {
            price    = m_myBagSelectedItem->GetPrice(cur);
            currency = cur;
            if (price > 0)
                break;
        }

        if (price <= 0)
        {
            ShowDialog(Constants::Messages::ItemShop::STANDARD_FACE_LOCKED, DialogStyle::Information);
            return;
        }

        // v3.50 and later:
        // const sf::String message = fmt::format(U"Item: {}\nPrice: {} {}\n\nAre you sure about selling the item?",
        //     m_myBagSelectedItem->GetName(), price, sf::String(std::string(magic_enum::enum_name(currency))));

        ShowDialog(Constants::Messages::ItemShop::SELL_CONFIRM, DialogStyle::OkCancel, [this] (auto accepted) { OnSellItemConfirmed(accepted); });
    }

    void StateItemShop::OnSellItemConfirmed(const bool accepted)
    {
        const auto sfxCancel = Instantiate<sf::Sound>(Sound::Effects::EF_03);

        if (!accepted)
        {
            m_mixer.Play(*sfxCancel, Sound::Channel::SFX);
            return;
        }

        std::size_t slotID = m_session.GetInventory().size() + 1;
        for (std::size_t i = 0; i < m_session.GetInventory().size(); i++)
        {
            if (m_session.GetInventory()[i] == m_myBagSelectedItem->GetID())
            {
                slotID = i;
                break;
            }
        }

        if (slotID >= m_session.GetInventory().size())
            return;

        if (Dispatch(ItemShopEvents::OnItemSell, ItemShopSellEventArgs{*m_myBagSelectedItem, slotID}))
            return;

        m_service.SellItem(SellItemRequest{static_cast<std::uint32_t>(slotID)}, [this] (const auto& ev)
        {
            OnSellItemResponded(ev);
        });
    }

    void StateItemShop::OnMyRoomButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        GetDirector().Present<StateMyRoom>();
    }

    void StateItemShop::OnShopMasterClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        m_shopMasterSpeechCounter++;
        if (m_shopMasterSpeechCounter >= m_shopMasterSpeech.size())
            m_shopMasterSpeechCounter = 0;

        for (std::size_t i = 0; i < m_shopMasterSpeech.size(); i++)
        {
            if (i == m_shopMasterSpeechCounter)
                m_mixer.Play(*m_shopMasterSpeech[i], Sound::Channel::SFX);
            else
                m_shopMasterSpeech[i]->stop();
        }
    }

    void StateItemShop::OnDefaultButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto avatar    = Instantiate<Avatar>(Resource::ItemShop::IDC_AVATAR);

        avatar->ClearEquipments();
        for (const auto id : m_session.GetEquippedItemIDs())
            avatar->Equip(m_items.Create(id));
    }

    void StateItemShop::OnShopCategoryCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev)
    {
        if (!sender.IsChecked())
            return;

        const auto sfxMenu = Instantiate<sf::Sound>(Sound::Effects::EF_11);
        auto category      = m_shopCategoryButtons.at(&sender);

        if (Dispatch(ItemShopEvents::OnMainCategoryChange, ItemShopMainCategoryEventArgs{category}))
            return;

        m_shopCategory = category;
        m_mixer.Play(*sfxMenu, Sound::Channel::SFX);

        for (const auto iterator : m_shopCategoryContainerMap)
        {
            iterator.second->SetEnabled(iterator.first == category);
            iterator.second->SetVisible(iterator.first == category);

            if (iterator.first != category)
                continue;

            for (const auto child : iterator.second->GetChildren())
            {
                if (const auto radio = dynamic_cast<Gx::RadioButton*>(child))
                {
                    if (radio->IsChecked())
                    {
                        m_itemCategory = m_itemCategoryMap.at(m_shopCategory).at(0);
                        InvalidateShopItemList(true);
                    }
                    else
                        radio->SetCheckedState(true);

                    break;
                }
            }
        }
    }

    void StateItemShop::OnItemCategoryCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev)
    {
        if (!sender.IsChecked())
            return;

        const auto sfxMenu        = Instantiate<sf::Sound>(Sound::Effects::EF_11);
        const auto& [category, i] = m_itemCategoryButtons.at(&sender);

        auto type = m_itemCategoryMap.at(category).at(i);
        if (Dispatch(ItemShopEvents::OnSubCategoryChange, ItemShopSubCategoryEventArgs{type}))
            return;

        m_itemCategory = type;
        m_mixer.Play(*sfxMenu, Sound::Channel::SFX);

        InvalidateShopItemList(true);
    }

    void StateItemShop::OnPlanetUpButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto sfxPlanet = Instantiate<sf::Sound>(Sound::Effects::EF_24_);
        const auto planet = Instantiate<Gx::Image>(Resource::ItemShop::IDC_IMAGE_PLANET);
        auto category = m_shopPlanetCategory;
        if (category == Planet::Unknown)
        {
            category = Planet::Event;
        }
        else if (planet->GetFrameCount() >= 12)
        {
            category = static_cast<Planet>(static_cast<std::uint8_t>(category) - 1);
        }
        else
        {
            auto categories = { Planet::Unknown, Planet::O2Planet, Planet::Aqua, Planet::Event };
            auto it = std::find_if(categories.begin(), categories.end(), [category] (const auto p) {
                return category == p;
            });

            if (it && it != categories.end())
                category = *(--it);
        }

        if (Dispatch(ItemShopEvents::OnPlanetFilterChange, ItemShopPlanetEventArgs{category}))
            return;

        m_shopPlanetCategory = category;
        m_mixer.Play(*sfxPlanet, Sound::Channel::SFX);
        InvalidateShopMaster(true);
        InvalidateShopItemList(true);
    }

    void StateItemShop::OnPlanetDownButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto sfxPlanet = Instantiate<sf::Sound>(Sound::Effects::EF_24_);
        const auto planet = Instantiate<Gx::Image>(Resource::ItemShop::IDC_IMAGE_PLANET);
        auto category = m_shopPlanetCategory;
        if (category == Planet::Event)
        {
            category = Planet::Unknown;
        }
        else if (planet->GetFrameCount() >= 12)
        {
            category = static_cast<Planet>(static_cast<std::uint8_t>(category) + 1);
        }
        else
        {
            auto categories = { Planet::Unknown, Planet::O2Planet, Planet::Aqua, Planet::Event };
            auto it = std::find_if(categories.begin(), categories.end(), [category] (const auto p) {
                return category == p;
            });

            if (it && it != categories.end())
                category = *(++it);
        }

        if (Dispatch(ItemShopEvents::OnPlanetFilterChange, ItemShopPlanetEventArgs{category}))
            return;

        m_shopPlanetCategory = category;
        m_mixer.Play(*sfxPlanet, Sound::Channel::SFX);
        InvalidateShopMaster(true);
        InvalidateShopItemList(true);
    }

    void StateItemShop::OnPlanetExtensionButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto sfxPlanet = Instantiate<sf::Sound>(Sound::Effects::EF_24_);
        auto p               = m_planetExtensionButtons.at(&sender);

        if (m_shopPlanetCategory == p)
            return;

        if (Dispatch(ItemShopEvents::OnPlanetFilterChange, ItemShopPlanetEventArgs{p}))
            return;

        m_shopPlanetCategory = p;
        m_mixer.Play(*sfxPlanet, Sound::Channel::SFX);
        InvalidateShopMaster(true);
        InvalidateShopItemList(true);
    }

    void StateItemShop::OnMaleButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        SelectGender(Gender::Female);
    }

    void StateItemShop::OnFemaleButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        SelectGender(Gender::Male);
    }

    void StateItemShop::SelectGender(Gender gender)
    {
        if (Dispatch(ItemShopEvents::OnGenderFilterChange, ItemShopGenderEventArgs{gender}))
            return;

        const auto sfxGender      = Instantiate<sf::Sound>(Sound::Effects::EF_15);
        const auto maleButton     = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_MALE);
        const auto femaleButton   = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_FEMALE);
        const auto disabledButton = gender == Gender::Female ? maleButton : femaleButton;
        const auto enabledButton  = gender == Gender::Female ? femaleButton : maleButton;

        m_genderCategory = m_genderCategory = gender;

        disabledButton->SetEnabled(false);
        disabledButton->SetVisible(false);

        enabledButton->SetEnabled(true);
        enabledButton->SetVisible(true);

        m_mixer.Play(*sfxGender, Sound::Channel::SFX);

        if (m_itemCategory == EquipmentType::Costume)
            InvalidateShopSetItemList(true);
        else
            InvalidateShopItemList(true);
    }

    void StateItemShop::OnShopScrollBarValueChanged(Gx::ScrollBar& sender, Gx::ScrollBar::ValueChangedEvent& ev)
    {
        const auto sfxPrev = Instantiate<sf::Sound>(Sound::Effects::EF_19_1);
        const auto sfxNext = Instantiate<sf::Sound>(Sound::Effects::EF_19_2);

        if (ev.Value < m_myBagCurrentPage)
            m_mixer.Play(*sfxPrev, Sound::Channel::SFX);
        else
            m_mixer.Play(*sfxNext, Sound::Channel::SFX);

        m_shopCurrentPage = static_cast<unsigned int>(ev.Value);
        InvalidateShopItemList();
    }

    void StateItemShop::OnShopScrollLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto itemScrollControls = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_ITEM_SCROLL_CONTROLS);
        const auto shopScrollBar      = itemScrollControls->FindChild<Gx::ScrollBar>(Resource::ItemShop::IDC_SCROLL_ITEM);

        shopScrollBar->Decrease();
    }

    void StateItemShop::OnShopScrollRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto itemScrollControls = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_ITEM_SCROLL_CONTROLS);
        const auto shopScrollBar      = itemScrollControls->FindChild<Gx::ScrollBar>(Resource::ItemShop::IDC_SCROLL_ITEM);

        shopScrollBar->Increase();
    }

    void StateItemShop::OnShopItemListScrolled(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto itemScrollControls = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_ITEM_SCROLL_CONTROLS);
        const auto shopScrollLeft     = itemScrollControls->FindChild<Gx::Button>(Resource::ItemShop::IDC_BUTTON_ITEM_SCROLL_LEFT);
        const auto shopScrollRight    = itemScrollControls->FindChild<Gx::Button>(Resource::ItemShop::IDC_BUTTON_ITEM_SCROLL_RIGHT);

        if (ev.Delta > 0)
            shopScrollRight->PerformClick();
        else
            shopScrollLeft->PerformClick();
    }

    void StateItemShop::OnMyBagScrollBarValueChanged(Gx::ScrollBar& sender, Gx::ScrollBar::ValueChangedEvent& ev)
    {
        const auto sfxPrev = Instantiate<sf::Sound>(Sound::Effects::EF_19_1);
        const auto sfxNext = Instantiate<sf::Sound>(Sound::Effects::EF_19_2);

        if (ev.Value < m_myBagCurrentPage)
            m_mixer.Play(*sfxPrev, Sound::Channel::SFX);
        else
            m_mixer.Play(*sfxNext, Sound::Channel::SFX);

        m_myBagCurrentPage = static_cast<unsigned int>(ev.Value);
        InvalidateMyBag();
    }

    void StateItemShop::OnMyBagScrollLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto myBagContainer    = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_MYBAG);
        const auto bagScrollControls = myBagContainer->FindChild<Gx::UiContainer>(Resource::ItemShop::MyBag::IDC_CONTAINER_MYBAG_SCROLL_CONTROLS);
        const auto bagScrollBar      = bagScrollControls->FindChild<Gx::ScrollBar>(Resource::ItemShop::MyBag::IDC_SCROLL_MYBAG);

        bagScrollBar->Decrease();
    }

    void StateItemShop::OnMyBagScrollRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto myBagContainer    = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_MYBAG);
        const auto bagScrollControls = myBagContainer->FindChild<Gx::UiContainer>(Resource::ItemShop::MyBag::IDC_CONTAINER_MYBAG_SCROLL_CONTROLS);
        const auto bagScrollBar      = bagScrollControls->FindChild<Gx::ScrollBar>(Resource::ItemShop::MyBag::IDC_SCROLL_MYBAG);

        bagScrollBar->Increase();
    }

    void StateItemShop::OnMyBagListScrolled(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto myBagContainer       = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_MYBAG);
        const auto bagScrollControls    = myBagContainer->FindChild<Gx::UiContainer>(Resource::ItemShop::MyBag::IDC_CONTAINER_MYBAG_SCROLL_CONTROLS);
        const auto bagScrollLeftButton  = bagScrollControls->FindChild<Gx::Button>(Resource::ItemShop::MyBag::IDC_BUTTON_MYBAG_SCROLL_LEFT);
        const auto bagScrollRightButton = bagScrollControls->FindChild<Gx::Button>(Resource::ItemShop::MyBag::IDC_BUTTON_MYBAG_SCROLL_RIGHT);

        if (ev.Delta > 0)
            bagScrollRightButton->PerformClick();
        else
            bagScrollLeftButton->PerformClick();
    }

    void StateItemShop::OnMyBagSlotClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto sfxClick = Instantiate<sf::Sound>(Sound::Effects::EF_25);
        const auto item     = m_myBagSlotItems.at(&sender);

        m_mixer.Play(*sfxClick, Sound::Channel::SFX);
        if (m_myBagSelectedItem == item || item->GetID() == 0)
            return;

        m_myBagSelectedItem = item;
        m_myBagSelectIndicator->SetVisible(true);

        sender.AddChild(*m_myBagSelectIndicator);
    }

    void StateItemShop::OnMyBagSlotDoubleClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto container = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_MYBAG);
        const auto item      = m_myBagSlotItems.at(&sender);
        const auto quantity  = m_myBagSlotQuantities.at(&sender);

        if (!item || item->GetID() == 0)
            return;

        if (item->GetType() == EquipmentType::AttributiveItem || quantity > 1)
        {
            if (const auto dialog = Instantiate<Gx::Dialog>(Resource::ItemShop::IDC_DIALOG_SKILL_INFO); dialog)
            {
                const auto nameLabel        = dialog->FindChild<Gx::Label>(Resource::ItemShop::SkillInfo::IDC_TEXT_ITEM_NAME);
                const auto quantityLabel    = dialog->FindChild<Gx::Label>(Resource::ItemShop::SkillInfo::IDC_TEXT_ITEM_QUANTITY);
                const auto skillLabel       = dialog->FindChild<Gx::Label>(Resource::ItemShop::SkillInfo::IDC_TEXT_ITEM_SKILL);
                const auto descriptionLabel = dialog->FindChild<Gx::Label>(Resource::ItemShop::SkillInfo::IDC_TEXT_ITEM_DESCRIPTION);
                const auto skillThumbnail   = dialog->FindChild<Gx::Image>(Resource::ItemShop::SkillInfo::IDC_IMAGE_ITEM_THUMBNAIL);

                nameLabel->SetString(item->GetName());
                quantityLabel->SetString(quantity > 0 ? std::to_string(quantity) : "-");
                skillLabel->SetString(item->GetName().substring(0, item->GetName().find(' ')));
                descriptionLabel->SetString(item->GetDescription());

                constexpr unsigned int bounds = 160;
                auto string = item->GetDescription();

                bool wrapped = true;
                while (wrapped)
                {
                    wrapped = false;

                    std::size_t checkpoint = 0;
                    for (const auto& glyph : descriptionLabel->GetShapedGlyphs())
                    {
                        const auto c = static_cast<std::size_t>(glyph.cluster);
                        if (c >= string.getSize() || string[c] == '\n')
                            continue;

                        if (string[c] == ' ')
                        {
                            checkpoint = c;
                            continue;
                        }

                        const auto position = descriptionLabel->GetTransform().transformPoint(glyph.position);
                        if (position.x > descriptionLabel->GetPosition().x + bounds)
                        {
                            if (string[checkpoint] == '\n')
                            {
                                checkpoint = 0;
                                continue;
                            }

                            string.replace(checkpoint, 1, "\n");
                            descriptionLabel->SetString(string);

                            wrapped = true;
                            break;
                        }
                    }
                }

                if (const auto texture = item->GetLargeThumbnail().GetTexture())
                    skillThumbnail->SetTexture(*texture, true);

                Present(*dialog, Gx::PresentationContext::Default);
            }
        }
        else
        {
            const auto sellButton = container->FindChild<Gx::Button>(Resource::ItemShop::MyBag::IDC_BUTTON_SELL);
            sellButton->PerformClick();
        }
    }

    void StateItemShop::OnCartPrevPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        if (m_cartCurrentPage > 0)
        {
            m_cartCurrentPage--;
            InvalidateCart();
        }
    }

    void StateItemShop::OnCartNextPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        m_cartCurrentPage++;
        InvalidateCart();
    }

    void StateItemShop::OnCartListScrolled(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto cartContainer      = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_CART);
        const auto cartPrevPageButton = cartContainer->FindChild<Gx::Button>(Resource::ItemShop::Cart::IDC_BUTTON_LEFT);
        const auto cartNextPageButton = cartContainer->FindChild<Gx::Button>(Resource::ItemShop::Cart::IDC_BUTTON_RIGHT);

        if (ev.Delta > 0)
            cartNextPageButton->PerformClick();
        else
            cartPrevPageButton->PerformClick();
    }

    void StateItemShop::OnCartItemDeleteButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto index = m_cartItemDeleteButtons.at(&sender);

        m_cart.Remove(index);
        InvalidateCart();
    }

    void StateItemShop::OnMyBagButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto myBagContainer = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_MYBAG);
        const auto cartContainer  = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_CART);

        myBagContainer->SetEnabled(true);
        myBagContainer->SetVisible(true);

        cartContainer->SetEnabled(false);
        cartContainer->SetVisible(false);
    }

    void StateItemShop::OnCartButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto myBagContainer = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_MYBAG);
        const auto cartContainer  = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_CART);

        myBagContainer->SetEnabled(false);
        myBagContainer->SetVisible(false);

        cartContainer->SetEnabled(true);
        cartContainer->SetVisible(true);
    }

    void StateItemShop::OnBackButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        if (const auto sfx = Find<sf::Sound>(Sound::Effects::EF_35))
            m_mixer.Play(*sfx, Sound::Channel::SFX);

        GetDirector().Dismiss<StateRoom>();
    }

    void StateItemShop::OnShopItemAddButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        auto metadata = m_shopItemAddButtons.at(&sender);
        if (O2::InInteropMode(InteropMode::Interface))
        {
            // Note: Removed client validation, let this validation runs on server side
            // if (m_session.GetInventory().size() >= 30)
            // {
            //     const auto it = std::find_if(m_session.GetInventory().begin(), m_session.GetInventory().end(), [itemID = metadata.ID] (auto id)
            //     {
            //         return id == 0;
            //     });
            //
            //     if (it == m_session.GetInventory().end())
            //     {
            //         ShowDialog(Constants::Messages::ItemShop::VACANT_SLOT_REQUIRED, DialogStyle::Information);
            //         return;
            //     }
            // }

            const auto it = std::find_if(m_session.GetInventory().begin(), m_session.GetInventory().end(), [itemID = metadata.ID] (auto id)
            {
                return id == itemID;
            });

            if (it != m_session.GetInventory().end())
                return;

            bool purchasable = false;
            for (auto c : { Currency::Gem, Currency::Cash })
            {
                const auto money = c == Currency::Gem ? m_session.GetWallet().Gem : m_session.GetWallet().Cash;
                if (auto pIt = metadata.Prices.find(c); pIt != metadata.Prices.end())
                {
                    if (pIt->second != 0 && pIt->second <= money)
                    {
                        purchasable = true;
                        break;
                    }
                }
            }

            if (!purchasable)
            {
                ShowDialog(Constants::Messages::ItemShop::NOT_ENOUGH_MONEY, DialogStyle::Information);
                return;
            }

            if (Dispatch(ItemShopEvents::OnItemPurchase, ItemShopPurchaseEventArgs{metadata}))
                return;

            m_service.PurchaseItem(PurchaseItemRequest{metadata.ID}, [=] (const auto& envelope)
            {
                OnPurchaseItemResponded(envelope, metadata);
            });

            return;
        }

        if (metadata.EquipmentType == EquipmentType::AttributiveItem)
        {
            ShowDialog("Skill item is currently not available", DialogStyle::Information);
            return;
        }

        const auto cartButton = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_CART);
        bool updated = false;
        if (metadata.EquipmentType == EquipmentType::Costume)
            updated = m_cart.AddEquipmentSet(metadata.ID);
        else
            updated = m_cart.AddEquipment(metadata.ID);

        if (updated)
        {
            m_cartCurrentPage = std::numeric_limits<std::uint32_t>::max();
            InvalidateCart();
        }

        cartButton->PerformClick();
    }

    void StateItemShop::OnShopItemPreviewButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto avatar          = Instantiate<Avatar>(Resource::ItemShop::IDC_AVATAR);
        const auto& [metadata, id] = m_shopItemPreviewButtons.at(&sender);
        if (metadata.Gender != Gender::Any && m_session.GetGender() != metadata.Gender)
        {
            ShowDialog(Constants::Messages::ItemShop::WRONG_GENDER, DialogStyle::Information);
            return;
        }

        avatar->Equip(m_items.Create(id));
    }

    void StateItemShop::OnShopSetItemAddButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto cartButton = Instantiate<Gx::Button>(Resource::ItemShop::IDC_BUTTON_CART);
        const auto& metadata  = m_shopSetItemAddButtons.at(&sender);
        if (m_cart.AddEquipmentSet(metadata.ID))
        {
            m_cartCurrentPage = std::numeric_limits<std::uint32_t>::max();
            InvalidateCart();
        }

        cartButton->PerformClick();
    }

    void StateItemShop::OnShopSetItemPreviewButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto currentAvatar   = Instantiate<Avatar>(Resource::ItemShop::IDC_AVATAR);
        const auto& [metadata, id] = m_shopSetItemPreviewButtons.at(&sender);
        if (metadata.Gender != Gender::Any && m_session.GetGender() != metadata.Gender)
        {
            ShowDialog(Constants::Messages::ItemShop::WRONG_GENDER, DialogStyle::Information);
            return;
        }

        currentAvatar->ClearEquipments();
        for (const auto& itemMetadata : m_shopSetItemList[id])
            currentAvatar->Equip(m_items.Create(itemMetadata.ID));
    }

    void StateItemShop::OnItemThumbnailFocusChanged(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto tooltip      = Instantiate<Gx::Image>(Resource::ItemShop::IDC_IMAGE_TOOLTIP);
        const auto& tooltipInfo = m_itemThumbnailTooltips.at(&sender);
        const auto& description = tooltipInfo.first;
        const auto slot         = tooltipInfo.second;
        Stop(m_tooltipDelay);
        if (sender.IsFocused() && m_tooltipDelay.GetState() != Gx::TaskState::Running && m_tooltipDelay.GetState() != Gx::TaskState::Completed)
        {
            m_tooltipDelay = Gx::Delay(sf::milliseconds(500), [=]
            {
                const auto message = tooltip->FindChild<Gx::Label>(Resource::ItemShop::IDC_TEXT_MESSAGE);
                message->SetString(description);
                message->SetLocalBounds(tooltip->GetLocalBounds());

                tooltip->SetVisible(slot->IsVisible());
                InvalidateShopMaster();
            });

            Run(m_tooltipDelay);
        }
        else
        {
            if (!sender.IsFocused())
                m_tooltipDelay.Reset();

            tooltip->SetVisible(false);
            InvalidateShopMaster();
        }
    }

    void StateItemShop::InvalidateShopMaster(const bool moveIn)
    {
        const auto tooltip = Instantiate<Gx::Image>(Resource::ItemShop::IDC_IMAGE_TOOLTIP);
        Gx::UiContainer* shopMaster = nullptr;

        for (auto [planet, master] : m_shopMasters)
        {
            if (master)
            {
                master->SetVisible(planet == m_shopPlanetCategory);
                if (planet == m_shopPlanetCategory)
                    shopMaster = master;
            }
        }

        shopMaster = shopMaster ? shopMaster : m_shopMasters[Planet::Unknown];
        if (shopMaster)
        {
            shopMaster->SetVisible(true);
            if (moveIn)
            {
                if (m_shopMasterEffect.has_value() && m_shopMasterEffect->GetState() != Gx::TaskState::Idle)
                {
                    Stop(*m_shopMasterEffect);
                    m_shopMasterEffect->Complete();
                }

                m_shopMasterEffect = Gx::Move(*shopMaster, shopMaster->GetPosition(), sf::seconds(0.15f));
                shopMaster->SetPosition(shopMaster->GetPosition().x - 100, shopMaster->GetPosition().y);
                m_shopMasterEffect->Update(sf::Time::Zero);

                m_shopMasterSpeechCounter = 0;
                Run(*m_shopMasterEffect);
            }

            if (const auto speech = shopMaster->FindChild<Gx::Animation>(Resource::ItemShop::ShopMaster::IDC_ANIMATION_SPEECH))
            {
                if (tooltip->IsVisible())
                    speech->Reset();
                else
                    speech->Stop();
            }
        }
    }

    void StateItemShop::InvalidateMyBag()
    {
        const auto avatar    = Instantiate<Avatar>(Resource::ItemShop::IDC_AVATAR);
        const auto sfxClick  = Instantiate<sf::Sound>(Sound::Effects::EF_25);
        const auto container = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_MYBAG);
        const auto bagList   = container->FindChild<Gx::List>(Resource::ItemShop::MyBag::IDC_LIST_BAG);
        const auto bagSlots  = bagList->GetChildren();

        if (m_inventory.empty())
        {
            for (const auto id : m_session.GetInventory())
                m_inventory.push_back(m_items.Create(id));
        }

        Gx::UiContainer* currentSlot = nullptr;
        unsigned int itemCount = 0;
        auto inventory = std::vector<Item*>();
        for (auto& item : m_inventory)
        {
            if (m_session.GetEquippedItemIDs().find(item.GetID()) == m_session.GetEquippedItemIDs().end())
                inventory.push_back(&item);
        }

        // TODO: Detect vertical count?
        constexpr unsigned int verticalCount = 2; // bagList->GetVerticalCount()
        for (std::size_t i = 0, j = m_myBagCurrentPage * verticalCount; i < bagSlots.size(); i++)
        {
            const auto slot = dynamic_cast<Gx::UiContainer*>(bagSlots[i]);
            if (!slot)
                continue;

            slot->SetVisible(false);
            slot->SetClickCallback(nullptr);
            slot->SetDoubleClickCallback(nullptr);
            if (j >= inventory.size())
                continue;

            const auto item = inventory[j++];
            unsigned int quantity = 0;

            if (const auto it = std::find(m_session.GetInventory().begin(), m_session.GetInventory().end(), item->GetID()); it != m_session.GetInventory().end())
                quantity = it->Quantity;

            currentSlot = item == m_myBagSelectedItem ? slot : currentSlot;
            const auto thumbnail = slot->FindChild<Gx::Image>(Resource::ItemShop::MyBag::Item::IDC_IMAGE_THUMBNAIL);
            if (item->GetID() == 0)
                thumbnail->SetTexCoords({});
            else if (item->GetSmallThumbnail().GetTexture())
                thumbnail->SetTexture(*item->GetSmallThumbnail().GetTexture(), true);
            else if (item->GetLargeThumbnail().GetTexture())
                thumbnail->SetTexture(*item->GetLargeThumbnail().GetTexture(), true);

            if (const auto quantityLabel = slot->FindChild<Gx::Label>(Resource::ItemShop::MyBag::Item::IDC_TEXT_QUANTITY))
            {
                if (quantity > 0)
                    quantityLabel->SetString(std::to_string(quantity));
                else
                    quantityLabel->SetString(std::string());
            }

            slot->SetVisible(true);
            m_myBagSlotItems[slot] = item;
            m_myBagSlotQuantities[slot] = quantity;
            slot->SetClickCallback([this] (auto& sender, auto& ev) { OnMyBagSlotClicked(sender, ev); });

            slot->SetDoubleClickCallback([this] (auto& sender, auto& ev) { OnMyBagSlotDoubleClicked(sender, ev); });
        }

        if (!currentSlot)
        {
            if (m_myBagSelectIndicator->GetParent())
                m_myBagSelectIndicator->GetParent()->RemoveChild(*m_myBagSelectIndicator);

            m_myBagSelectIndicator->SetVisible(false);
        }
        else
        {
            currentSlot->AddChild(*m_myBagSelectIndicator);
            m_myBagSelectIndicator->SetVisible(true);
        }

        const auto bagScrollControls = container->FindChild<Gx::UiContainer>(Resource::ItemShop::MyBag::IDC_CONTAINER_MYBAG_SCROLL_CONTROLS);
        const auto bagScrollBar = bagScrollControls->FindChild<Gx::ScrollBar>(Resource::ItemShop::MyBag::IDC_SCROLL_MYBAG);
        bagScrollBar->SetMaximumValue(inventory.size() < bagSlots.size() ? 0 : static_cast<int>(std::ceil(static_cast<float>(inventory.size() - bagSlots.size()) / verticalCount)));

        const auto currentGem = Instantiate<Gx::BitmapNumber>(Resource::ItemShop::IDC_NUMBER_GEM);
        currentGem->SetValue(m_session.GetWallet().Gem);

        const auto currentCash = Instantiate<Gx::BitmapNumber>(Resource::ItemShop::IDC_NUMBER_CASH);
        currentCash->SetValue(m_session.GetWallet().Cash);
    }

    void StateItemShop::InvalidateCart()
    {
        const auto container = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_CART);
        const auto cartList  = container->FindChild<Gx::List>(Resource::ItemShop::Cart::IDC_LIST_CART);
        const auto slots     = cartList->GetChildren();
        const auto cartItems = m_cart.GetItems();
        const int maxPage    = static_cast<unsigned int>(std::ceil(static_cast<float>(cartItems.size()) / static_cast<float>(slots.size())));

        m_cartCurrentPage = m_cartCurrentPage >= maxPage ? maxPage - 1 : m_cartCurrentPage;
        const auto& itemData = m_items.GetItemData();
        const auto& setInfoData = m_items.GetSetInfoData();

        unsigned int gem = 0, cash = 0;
        const auto calculateItem = [&] (const unsigned int itemID)
        {
            if (const auto it = itemData.Items.find(itemID); it != itemData.Items.end())
            {
                // Item or Set with one single costume item
                auto metadata = it->second;
                if (const auto gemPrice = metadata.Prices.find(Currency::Gem); gemPrice != metadata.Prices.end())
                    gem += gemPrice->second;
                else if (const auto cashPrice = metadata.Prices.find(Currency::Cash); cashPrice != metadata.Prices.end())
                    cash += cashPrice->second;
            }
        };

        for (auto item : cartItems)
        {
            if (item.Type == CartItemType::EquipmentSet)
            {
                const auto& sets = setInfoData.Sets;
                if (auto set = sets.find(item.ID); set != sets.end())
                {
                    for (const auto itemID : set->second.ItemsIDs)
                    {
                        calculateItem(itemID);

                        if (set->second.Discounts.empty())
                            continue;

                        auto discounts = set->second.Discounts.find(itemID);
                        if (discounts == set->second.Discounts.end())
                            continue;

                        if (auto gemDiscount = discounts->second.find(Currency::Gem); gemDiscount != discounts->second.end())
                            gem = gem < gemDiscount->second ? 0 : gem - gemDiscount->second;
                        else if (auto cashDiscount = discounts->second.find(Currency::Cash); cashDiscount != discounts->second.end())
                            cash = cash < cashDiscount->second ? 0 : cash - cashDiscount->second;
                    }
                }
            }
            if (item.Type == CartItemType::Equipment)
                calculateItem(item.ID);

            // TODO: Music
        }

        for (std::size_t i = 0, j = m_cartCurrentPage * slots.size(); i < slots.size(); i++)
        {
            const auto slot = dynamic_cast<Gx::UiContainer*>(slots[i]);
            if (!slot)
                continue;

            if (j >= cartItems.size())
            {
                slot->SetEnabled(false);
                slot->SetVisible(false);
                continue;
            }

            slot->SetEnabled(true);
            slot->SetVisible(true);

            auto item               = cartItems[j++];
            const auto id           = slot->FindChild<Gx::Label>(Resource::ItemShop::Cart::Item::IDC_TEXT_NUMBER);
            const auto name         = slot->FindChild<Gx::Label>(Resource::ItemShop::Cart::Item::IDC_TEXT_NAME);
            const auto type         = slot->FindChild<Gx::Image>(Resource::ItemShop::Cart::Item::IDC_IMAGE_ITEM_TYPE);
            const auto price        = slot->FindChild<Gx::Label>(Resource::ItemShop::Cart::Item::IDC_TEXT_PRICE);
            const auto deleteButton = slot->FindChild<Gx::Button>(Resource::ItemShop::Cart::Item::IDC_BUTTON_DELETE);

            id->SetString(std::to_string(j));
            if (item.Type == CartItemType::EquipmentSet)
            {
                const auto& sets = setInfoData.Sets;
                if (auto set = sets.find(item.ID); set != sets.end())
                {
                    name->SetString(set->second.Name);
                    unsigned int setPriceGem  = 0;
                    unsigned int setPriceCash = 0;

                    for (const auto itemID : set->second.ItemsIDs)
                    {
                        if (auto it = itemData.Items.find(itemID); it != itemData.Items.end())
                        {
                            auto metadata = it->second;
                            if (auto gemPrice = metadata.Prices.find(Currency::Gem); gemPrice != metadata.Prices.end())
                                setPriceGem += gemPrice->second;
                            else if (auto cashPrice = metadata.Prices.find(Currency::Cash); cashPrice != metadata.Prices.end())
                                setPriceCash += cashPrice->second;

                            if (set->second.Discounts.empty())
                                continue;

                            auto discounts = set->second.Discounts.find(itemID);
                            if (discounts == set->second.Discounts.end())
                                continue;

                            if (auto gemDiscount = discounts->second.find(Currency::Gem); gemDiscount != discounts->second.end())
                                setPriceGem = setPriceGem < gemDiscount->second ? 0 : setPriceGem - gemDiscount->second;
                            else if (auto cashDiscount = discounts->second.find(Currency::Cash); cashDiscount != discounts->second.end())
                                setPriceCash = setPriceCash < cashDiscount->second ? 0 : setPriceCash - cashDiscount->second;
                        }
                    }

                    if (setPriceGem > 0)
                        price->SetString(fmt::format("{}G", setPriceGem));
                    else
                        price->SetString(fmt::format("{}M", setPriceCash));
                }

                type->SetFrame("EquipmentSet");
            }

            if (item.Type == CartItemType::Equipment)
            {
                if (auto it = itemData.Items.find(item.ID); it != itemData.Items.end())
                {
                    auto metadata = it->second;
                    name->SetString(metadata.Name);
                    if (auto gemPrice = metadata.Prices.find(Currency::Gem); gemPrice != metadata.Prices.end())
                        price->SetString(fmt::format("{}G", gemPrice->second));
                    else if (auto cashPrice = metadata.Prices.find(Currency::Cash); cashPrice != metadata.Prices.end())
                        price->SetString(fmt::format("{}M", cashPrice->second));
                }

                type->SetFrame("Equipment");
            }
            else if (item.Type == CartItemType::Music)
            {
                type->SetFrame("Music");
            }

            m_cartItemDeleteButtons[deleteButton] = j - 1;
            deleteButton->SetClickCallback([this] (auto& sender, auto& ev) { OnCartItemDeleteButtonClicked(sender, ev); });
        }

        const auto currentPage = container->FindChild<Gx::BitmapNumber>(Resource::ItemShop::Cart::IDC_NUMBER_CURRENT_PAGE);
        const auto totalPage   = container->FindChild<Gx::BitmapNumber>(Resource::ItemShop::Cart::IDC_NUMBER_MAX_PAGE);
        const auto totalGem    = container->FindChild<Gx::BitmapNumber>(Resource::ItemShop::Cart::IDC_NUMBER_TOTAL_GEM);
        const auto totalCash   = container->FindChild<Gx::BitmapNumber>(Resource::ItemShop::Cart::IDC_NUMBER_TOTAL_CASH);

        currentPage->SetValue(maxPage > 0 ? m_cartCurrentPage + 1 : 0);
        totalPage->SetValue(maxPage);
        totalGem->SetValue(gem);
        totalCash->SetValue(cash);
    }

    void StateItemShop::InvalidateShopItemList(const bool rebuildList)
    {
        const auto avatar           = Instantiate<Avatar>(Resource::ItemShop::IDC_AVATAR);
        const auto planet           = Instantiate<Gx::Image>(Resource::ItemShop::IDC_IMAGE_PLANET);
        const auto itemList         = Instantiate<Gx::List>(Resource::ItemShop::IDC_LIST_ITEM);
        const auto slots            = itemList->GetChildren();
        const std::size_t slotCount = slots.size();
        const auto scrollControls   = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_ITEM_SCROLL_CONTROLS);
        const auto shopScrollBar    = scrollControls->FindChild<Gx::ScrollBar>(Resource::ItemShop::IDC_SCROLL_ITEM);

        Instantiate<Gx::Image>(Resource::ItemShop::IDC_IMAGE_TOOLTIP)->SetVisible(false);
        if (m_itemCategory == EquipmentType::Costume)
        {
            InvalidateShopSetItemList(rebuildList);
            return;
        }

        const auto setItemContainer = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_SET_ITEM);
        itemList->SetVisible(true);
        itemList->SetEnabled(true);
        setItemContainer->SetVisible(false);
        setItemContainer->SetEnabled(false);

        if (m_shopPlanetCategory == Planet::Unknown)
            planet->SetFrame("ShowAll");
        else
            planet->SetFrame(std::string(magic_enum::enum_name(m_shopPlanetCategory)));

        if (rebuildList)
        {
            m_shopItemList.clear();
            for (auto it = m_items.GetItemData().Items.rbegin(); it != m_items.GetItemData().Items.rend(); ++it)
            {
                auto& header = it->second;

                // Check price
                if (header.Prices.find(Currency::Gem) == header.Prices.end() && header.Prices.find(Currency::Cash) == header.Prices.end())
                    continue;

                // Check Category
                if (header.EquipmentType != m_itemCategory)
                    continue;

                // Check Gender
                if (header.Gender != m_genderCategory && header.Gender != Gender::Any)
                    continue;

                // Check Planet
                if (m_shopPlanetCategory != Planet::Unknown && header.Origin != Planet::Unknown && m_shopPlanetCategory != header.Origin)
                    continue;

                m_shopItemList.push_back(header);
            }
        }

        // TODO: Detect vertical count?
        const auto scrollValue = shopScrollBar->GetValue();
        constexpr int verticalCount = 2; //itemList->GetVerticalCount()

        shopScrollBar->SetMaximumValue(m_shopItemList.size() < slots.size() ? 0 : static_cast<int>(std::ceil(static_cast<float>(m_shopItemList.size() - slots.size()) / verticalCount)));
        if (rebuildList && shopScrollBar->GetValue() != 0)
        {
            // This causes 2 times invalidation, but it can't be that bad, right?
            shopScrollBar->SetValue(0); // This must trigger invalidate;
            return;
        }

        if (scrollValue != shopScrollBar->GetValue())
            return; // Already invalidated via SetMaximumValue (which trigger SetValue)

        for (std::size_t i = 0, j = m_shopCurrentPage * verticalCount; i < slots.size(); i++)
        {
            const auto slot = dynamic_cast<Gx::UiContainer*>(slots[i]);
            if (!slot)
                continue;

            if (j >= m_shopItemList.size() || i >= slotCount)
            {
                slot->SetEnabled(false);
                slot->SetVisible(false);
                continue;
            }

            auto metadata = m_shopItemList[j++];
            auto currency = Currency::Gem;
            auto price    = 0;
            for (auto cur : { Currency::Gem, Currency::Cash })
            {
                price    = metadata.Prices[cur];
                currency = cur;
                if (price > 0)
                    break;
            }

            slot->SetEnabled(true);
            slot->SetVisible(true);

            const auto name          = slot->FindChild<Gx::Label>(Resource::ItemShop::ItemList::IDC_TEXT_NAME);
            const auto priceTag      = slot->FindChild<Gx::BitmapNumber>(Resource::ItemShop::ItemList::IDC_NUMBER_PRICE);
            const auto currencyTag   = slot->FindChild<Gx::Image>(Resource::ItemShop::ItemList::IDC_IMAGE_CURRENCY);
            const auto addButton     = slot->FindChild<Gx::Button>(Resource::ItemShop::ItemList::IDC_BUTTON_ADD);
            const auto previewButton = slot->FindChild<Gx::Button>(Resource::ItemShop::ItemList::IDC_BUTTON_PREVIEW);
            const auto thumbnail     = slot->FindChild<Gx::Image>(Resource::ItemShop::ItemList::IDC_IMAGE_ITEM);

            name->SetString(metadata.Name);
            priceTag->SetValue(price);

            if (currency == Currency::Gem)
                currencyTag->SetFrame("Gem");
            else
                currencyTag->SetFrame("Cash");

            currencyTag->SetPosition({
                priceTag->GetPosition().x - priceTag->GetLocalBounds().size.x - currencyTag->GetLocalBounds().size.x - 1,
                currencyTag->GetPosition().y
            });


            m_shopItemAddButtons[addButton] = metadata;
            addButton->SetClickCallback([this] (auto& sender, auto& ev) { OnShopItemAddButtonClicked(sender, ev); });

            m_shopItemPreviewButtons[previewButton] = { metadata, metadata.ID };
            previewButton->SetClickCallback([this] (auto& sender, auto& ev) { OnShopItemPreviewButtonClicked(sender, ev); });

            thumbnail->SetVisible(false);
            thumbnail->SetFocusChangedCallback(nullptr);

            if (m_thumbnails.find(metadata.ID) == m_thumbnails.end())
                m_thumbnails[metadata.ID] = std::move(m_items.Create(metadata.ID));

            const auto item = m_thumbnails[metadata.ID];
            if (item.GetID() == 0)
                continue;

            const auto sprite = item.GetLargeThumbnail();
            if (!sprite.GetTexture())
                continue;

            thumbnail->SetVisible(true);
            thumbnail->SetTexture(*sprite.GetTexture());
            thumbnail->SetTexCoords(sprite.GetTexCoords());

            m_itemThumbnailTooltips[thumbnail] = { metadata.Description, slot };
            thumbnail->SetFocusChangedCallback([this] (auto& sender, auto& ev) { OnItemThumbnailFocusChanged(sender, ev); });

            if (thumbnail->IsFocused())
            {
                // Force re-focus
                thumbnail->SetFocus(false);
                thumbnail->SetFocus(true);
                InvalidateShopMaster();
            }
        }
    }

    void StateItemShop::InvalidateShopSetItemList(bool rebuildList)
    {
        const auto currentAvatar    = Instantiate<Avatar>(Resource::ItemShop::IDC_AVATAR);
        const auto planet           = Instantiate<Gx::Image>(Resource::ItemShop::IDC_IMAGE_PLANET);
        const auto itemList         = Instantiate<Gx::List>(Resource::ItemShop::IDC_LIST_ITEM);
        const auto scrollControls   = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_ITEM_SCROLL_CONTROLS);
        const auto shopScrollBar    = scrollControls->FindChild<Gx::ScrollBar>(Resource::ItemShop::IDC_SCROLL_ITEM);
        const auto setItemContainer = Instantiate<Gx::UiContainer>(Resource::ItemShop::IDC_CONTAINER_SET_ITEM);
        const auto setItemList      = setItemContainer->FindChild<Gx::List>(Resource::ItemShop::IDC_LIST_SET_ITEM);
        const auto slots            = setItemList->GetChildren();
        const std::size_t slotCount = setItemList->GetChildrenCount();

        itemList->SetVisible(false);
        itemList->SetEnabled(false);
        setItemContainer->SetVisible(true);
        setItemContainer->SetEnabled(true);

        if (m_shopPlanetCategory == Planet::Unknown)
            planet->SetFrame("ShowAll");
        else
            planet->SetFrame(std::string(magic_enum::enum_name(m_shopPlanetCategory)));

        const auto setInfoData = m_items.GetSetInfoData();
        const auto itemData    = m_items.GetItemData();
        if (rebuildList)
        {
            m_shopSetList.clear();
            m_shopSetItemList.clear();
            m_shopSetItemPrices.clear();
            const auto sets = setInfoData.Sets;
            for (auto set = sets.rbegin(); set != sets.rend(); ++set)
            {
                const auto& header = set->second;

                // Check item set list
                if (header.ID == 0 || header.ItemsIDs.empty())
                    continue;

                // Check Gender
                if (header.Gender != m_genderCategory && header.Gender != Gender::Any)
                    continue;

                // Check Planet
                if (m_shopPlanetCategory != Planet::Unknown && header.Origin != Planet::Unknown && m_shopPlanetCategory != header.Origin)
                    continue;

                // Calculate prices
                auto setItems = std::vector<ItemMetadata>();
                auto prices   = std::unordered_map<Currency, unsigned int>();
                auto priceEnabled = std::unordered_map<Currency, bool>{ { Currency::Gem, true}, {Currency::Cash, true} };
                for (unsigned int itemID : header.ItemsIDs)
                {
                    if (auto it = itemData.Items.find(itemID); it != itemData.Items.end())
                    {
                        for (const auto& [c, p] : it->second.Prices)
                        {
                            // The all items in the set must be buyable with the currency
                            priceEnabled[c] = priceEnabled[c] && p > 0;
                            if (!priceEnabled[c])
                            {
                                prices[c] = 0;
                                continue;
                            }

                            prices[c] += p;
                        }

                        setItems.push_back(it->second);
                    }
                }

                // Check contents of set items
                if (setItems.empty())
                    continue;

                // Check price
                if (prices.find(Currency::Gem) == prices.end() && prices.find(Currency::Cash) == prices.end())
                    continue;

                m_shopSetList.push_back(header);
                m_shopSetItemList[header.ID] = setItems;
                m_shopSetItemPrices[header.ID] = prices;
            }
        }

        // TODO: Detect vertical count?
        const auto scrollValue = shopScrollBar->GetValue();
        constexpr int verticalCount = 1; // setItemList->GetVerticalCount()
        shopScrollBar->SetMaximumValue(m_shopSetList.size() < slots.size() ? 0 : static_cast<int>(std::ceil(static_cast<float>(m_shopSetList.size() - slots.size()) / verticalCount)));
        if (rebuildList && shopScrollBar->GetValue() != 0)
        {
            // This causes 2 times invalidation, but it can't be that bad, right?
            shopScrollBar->SetValue(0); // This must trigger invalidate;
            return;
        }

        if (scrollValue != shopScrollBar->GetValue())
            return; // Already invalidated via SetMaximumValue (which trigger SetValue)

        for (std::size_t i = 0, j = m_shopCurrentPage * verticalCount; i < slots.size(); i++)
        {
            const auto slot = dynamic_cast<Gx::UiContainer*>(slots[i]);
            if (!slot)
                continue;

            if (j >= m_shopSetItemList.size() || i >= slotCount)
            {
                slot->SetEnabled(false);
                slot->SetVisible(false);
                continue;
            }

            auto metadata = m_shopSetList[j++];
            const auto itemSetList = m_shopSetItemList[metadata.ID];

            auto currency = Currency::Gem;
            auto price    = 0;
            for (auto cur : { Currency::Gem, Currency::Cash })
            {
                price    = m_shopSetItemPrices[metadata.ID][cur];
                currency = cur;
                if (price > 0)
                    break;
            }

            slot->SetEnabled(true);
            slot->SetVisible(true);

            const auto name          = slot->FindChild<Gx::Label>(Resource::ItemShop::SetItemList::IDC_TEXT_NAME);
            const auto avatar        = slot->FindChild<Avatar>(Resource::ItemShop::SetItemList::IDC_AVATAR);
            const auto thumbnail     = slot->FindChild<Gx::Image>(Resource::ItemShop::SetItemList::IDC_IMAGE_ITEM);
            const auto pieceList     = slot->FindChild<Gx::List>(Resource::ItemShop::SetItemList::IDC_LIST_ITEM_PIECE);
            const auto priceTag      = slot->FindChild<Gx::BitmapNumber>(Resource::ItemShop::SetItemList::IDC_NUMBER_PRICE);
            const auto currencyTag   = slot->FindChild<Gx::Image>(Resource::ItemShop::SetItemList::IDC_IMAGE_CURRENCY);
            const auto addButton     = slot->FindChild<Gx::Button>(Resource::ItemShop::SetItemList::IDC_BUTTON_ADD);
            const auto previewButton = slot->FindChild<Gx::Button>(Resource::ItemShop::SetItemList::IDC_BUTTON_PREVIEW);

            name->SetString(metadata.Name);
            avatar->SetGender(m_genderCategory);

            for (auto& [_, item] : m_items.GetDefaultItems(avatar->GetGender()))
                avatar->SetDefaultItem(std::move(item));

            avatar->ClearEquipments();
            for (const auto& itemMetadata : itemSetList)
                avatar->Equip(m_items.Create(itemMetadata.ID));

            priceTag->SetValue(price);

            if (currency == Currency::Gem)
                currencyTag->SetFrame("Gem");
            else
                currencyTag->SetFrame("Cash");

            currencyTag->SetPosition({
                priceTag->GetPosition().x - priceTag->GetLocalBounds().size.x - currencyTag->GetLocalBounds().size.x - 1,
                currencyTag->GetPosition().y
            });

            m_shopSetItemAddButtons[addButton] = metadata;
            addButton->SetClickCallback([this] (auto& sender, auto& ev) { OnShopSetItemAddButtonClicked(sender, ev); });

            m_shopSetItemPreviewButtons[previewButton] = { metadata, metadata.ID };
            previewButton->SetClickCallback([this] (auto& sender, auto& ev) { OnShopSetItemPreviewButtonClicked(sender, ev); });

            const auto pieces = pieceList->GetChildren();
            for (std::size_t p = 0; p < pieces.size(); p++)
            {
                const auto pieceName = dynamic_cast<Gx::Label*>(pieces[p]);
                if (!pieceName)
                    continue;

                pieceName->SetVisible(true);
                pieceName->SetString(std::to_string(p + 1));
                if (p < itemSetList.size())
                    pieceName->SetString(fmt::format(U"{} {}", p + 1, itemSetList[p].Name));
            }

            thumbnail->SetVisible(false);
            m_itemThumbnailTooltips[thumbnail] = { metadata.Description, slot };
            thumbnail->SetFocusChangedCallback([this] (auto& sender, auto& ev) { OnItemThumbnailFocusChanged(sender, ev); });

            if (thumbnail->IsFocused())
            {
                // Force re-focus
                thumbnail->SetFocus(false);
                thumbnail->SetFocus(true);
                InvalidateShopMaster();
            }
        }
    }
}
