#include <emmintrin.h>
#include <future>
#include <OTwo/States/StateItemShop.hpp>
#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StateMyRoom.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/CartContext.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/ScrollBar.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/RadioButton.hpp>

#include <magic_enum.hpp>

StateItemShop::StateItemShop(Gx::Mixer& mixer, SessionContext& session, CartContext& cart, ItemFactory& items) :
    m_mixer(mixer),
    m_session(session),
    m_cart(cart),
    m_items(items),
    m_myBagSelectedItem(),
    m_myBagSelectIndicator()
{
}

void StateItemShop::Initialize()
{
    State::Initialize();

    const auto& player   = m_session.GetCurrentPlayer();
    const auto bgm       = Instantiate<sf::Music>("BGM/bgItemShop.ogg");
    const auto sfxAccept = Instantiate<sf::Sound>("bgEffect/02");
    const auto sfxCancel = Instantiate<sf::Sound>("bgEffect/03");
    const auto sfxPrev   = Instantiate<sf::Sound>("bgEffect/19_1");
    const auto sfxNext   = Instantiate<sf::Sound>("bgEffect/19_2");
    const auto sfxMenu   = Instantiate<sf::Sound>("bgEffect/11");
    const auto sfxGender = Instantiate<sf::Sound>("bgEffect/15");
    const auto sfxPlanet = Instantiate<sf::Sound>("bgEffect/24_");

    m_genderCategory = player.Gender;
    m_shopCategory   = ShopCategory::Special;
    m_itemCategory   = EquipmentType::Costume;

    const auto avatar = Instantiate<Avatar>("IDC_AVATAR");
    avatar->SetGender(player.Gender);
    for (auto [_, item] : m_items.GetDefaultItems(player.Gender))
        avatar->SetDefaultItem(item);

    for (const auto id : player.EquippedItemIDs)
    {
        if (const auto item = m_items.GetItem(id); item)
            avatar->Equip(item);
    }

    const auto nicknameText = Instantiate<Gx::Label>("IDC_TEXT_NICKNAME");
    nicknameText->SetString("Lv." + std::to_string(player.Level) + ": " + player.Name);

    const auto currentGem = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_GEM");
    currentGem->SetValue(player.Gem);

    const auto currentCash = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_CASH");
    currentCash->SetValue(player.Cash);

    const auto myRoomButton = Instantiate<Gx::Button>("IDC_BUTTON_MY_ROOM");
    myRoomButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateMyRoom>();
    });

    const auto tooltip = Instantiate<Gx::Image>("IDC_IMAGE_TOOLTIP");
    tooltip->SetVisible(false);

    const auto defaultButton = Instantiate<Gx::Button>("IDC_BUTTON_DEFAULT");
    defaultButton->SetClickCallback([=] (auto&, auto&)
    {
        avatar->ClearEquipments();
        for (const auto id : player.EquippedItemIDs)
        {
            if (const auto item = m_items.GetItem(id); item)
                avatar->Equip(item);
        }
    });

    const auto categoryButtonsContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_CATEGORY_BUTTONS");
    const std::unordered_map<ShopCategory, Gx::RadioButton*> shopCategoryButtonMap =
    {
         { ShopCategory::Special,    categoryButtonsContainer->FindChild<Gx::RadioButton>("IDC_BUTTON_SPECIAL") },
         { ShopCategory::Fashion,    categoryButtonsContainer->FindChild<Gx::RadioButton>("IDC_BUTTON_FASHION") },
         { ShopCategory::Accessory,  categoryButtonsContainer->FindChild<Gx::RadioButton>("IDC_BUTTON_ACCESSORY") },
         { ShopCategory::Beauty,     categoryButtonsContainer->FindChild<Gx::RadioButton>("IDC_BUTTON_BEAUTY") },
         { ShopCategory::Instrument, categoryButtonsContainer->FindChild<Gx::RadioButton>("IDC_BUTTON_INSTRUMENT") },
    };

    const std::unordered_map<ShopCategory, Gx::UiContainer*> shopCategoryContainerMap =
    {
        { ShopCategory::Special,    Instantiate<Gx::UiContainer>("IDC_CONTAINER_SPECIAL_CATEGORY") },
        { ShopCategory::Fashion,    Instantiate<Gx::UiContainer>("IDC_CONTAINER_FASHION_CATEGORY") },
        { ShopCategory::Accessory,  Instantiate<Gx::UiContainer>("IDC_CONTAINER_ACCESSORY_CATEGORY") },
        { ShopCategory::Beauty,     Instantiate<Gx::UiContainer>("IDC_CONTAINER_BEAUTY_CATEGORY") },
        { ShopCategory::Instrument, Instantiate<Gx::UiContainer>("IDC_CONTAINER_INSTRUMENT_CATEGORY") },
    };

    const std::unordered_map<ShopCategory, std::vector<EquipmentType>> itemCategoryMap =
    {
        { ShopCategory::Special, { EquipmentType::Costume, EquipmentType::Wings, EquipmentType::Pet, EquipmentType::AttributiveItem } },
        { ShopCategory::Fashion, { EquipmentType::Top, EquipmentType::Pants, EquipmentType::ClothesAccessories, EquipmentType::Shoes } },
        { ShopCategory::Accessory, { EquipmentType::Accessories, EquipmentType::Earrings, EquipmentType::Necklace, EquipmentType::Glasses, EquipmentType::Gloves } },
        { ShopCategory::Beauty, { EquipmentType::Hair, EquipmentType::HairAccessories, EquipmentType::Face } },
        { ShopCategory::Instrument, { EquipmentType::Guitar, EquipmentType::Bass, EquipmentType::Keyboard, EquipmentType::Drum, EquipmentType::InstrumentAccessories } },
    };

    for (auto [category, button] : shopCategoryButtonMap)
    {
        shopCategoryContainerMap.at(category)->SetEnabled(category == ShopCategory::Special);
        shopCategoryContainerMap.at(category)->SetVisible(category == ShopCategory::Special);

        button->SetCheckedState(category == ShopCategory::Special);
        button->SetCheckStateChangeCallback([this, sfxMenu, category, shopCategoryContainerMap, itemCategoryMap] (auto& sender)
        {
            if (!sender.IsChecked())
                return;

            m_shopCategory = category;
            m_mixer.Play(sfxMenu);

            for (const auto iterator : shopCategoryContainerMap)
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
                            m_itemCategory = itemCategoryMap.at(m_shopCategory).at(0);
                            InvalidateShopItemList(true);
                        }
                        else
                            radio->SetCheckedState(true);

                        break;
                    }
                }
            }
        });

        auto children = shopCategoryContainerMap.at(category)->GetChildren();
        for (std::size_t i = 0; i < children.size(); i++)
        {
            if (const auto radio = dynamic_cast<Gx::RadioButton*>(children[i]))
            {
                radio->SetCheckedState(category == ShopCategory::Special && i == 0);
                radio->SetCheckStateChangeCallback([this, i, sfxMenu, category, itemCategoryMap] (auto& sender)
                {
                    if (!sender.IsChecked())
                        return;

                    m_itemCategory = itemCategoryMap.at(category).at(i);
                    m_mixer.Play(sfxMenu);

                    InvalidateShopItemList(true);
                });
            }
        }
    }

    const auto planetPrevButton = Instantiate<Gx::Button>("IDC_BUTTON_PLANET_UP");
    const auto planetNextButton = Instantiate<Gx::Button>("IDC_BUTTON_PLANET_DOWN");

    planetPrevButton->SetClickCallback([=] (auto&, auto&)
    {
        if (m_shopPlanetCategory == Planet::Unknown)
            m_shopPlanetCategory = Planet::Event;
        else
            m_shopPlanetCategory = static_cast<Planet>(static_cast<std::uint8_t>(m_shopPlanetCategory) - 1);

        m_mixer.Play(sfxPlanet);
        InvalidateShopItemList(true);
    });

    planetNextButton->SetClickCallback([=] (auto&, auto&)
    {
        if (m_shopPlanetCategory == Planet::Event)
            m_shopPlanetCategory = Planet::Unknown;
        else
            m_shopPlanetCategory = static_cast<Planet>(static_cast<std::uint8_t>(m_shopPlanetCategory) + 1);

        m_mixer.Play(sfxPlanet);
        InvalidateShopItemList(true);
    });

    const auto maleButton   = Instantiate<Gx::Button>("IDC_BUTTON_MALE");
    const auto femaleButton = Instantiate<Gx::Button>("IDC_BUTTON_FEMALE");

    maleButton->SetEnabled(m_genderCategory == Gender::Male);
    maleButton->SetVisible(m_genderCategory == Gender::Male);
    femaleButton->SetEnabled(m_genderCategory == Gender::Female);
    femaleButton->SetVisible(m_genderCategory == Gender::Female);

    maleButton->SetClickCallback([=] (auto&, auto&)
    {
        m_genderCategory = m_genderCategory = Gender::Female;

        maleButton->SetEnabled(false);
        maleButton->SetVisible(false);

        femaleButton->SetEnabled(true);
        femaleButton->SetVisible(true);

        m_mixer.Play(sfxGender);

        if (m_itemCategory == EquipmentType::Costume)
            InvalidateShopSetItemList(true);
        else
            InvalidateShopItemList(true);
    });

    femaleButton->SetClickCallback([=] (auto&, auto&)
    {
        m_genderCategory = m_genderCategory = Gender::Male;

        femaleButton->SetEnabled(false);
        femaleButton->SetVisible(false);

        maleButton->SetEnabled(true);
        maleButton->SetVisible(true);

        m_mixer.Play(sfxGender);
        if (m_itemCategory == EquipmentType::Costume)
            InvalidateShopSetItemList(true);
        else
            InvalidateShopItemList(true);
    });

    m_shopCurrentPage = 0;
    const auto shopScrollBar = Instantiate<Gx::ScrollBar>("IDC_SCROLL_ITEM");
    shopScrollBar->SetMaximumValue(std::floor(static_cast<float>(m_shopItemList.size()) / 8.f));
    shopScrollBar->SetValueChangedCallback([this, sfxPrev, sfxNext] (auto&, const float value)
    {
        if (value < m_myBagCurrentPage)
            m_mixer.Play(sfxPrev, "SFX");
        else
            m_mixer.Play(sfxNext, "SFX");

        m_shopCurrentPage = static_cast<unsigned int>(value);
        InvalidateShopItemList();
    });

    const auto shopScrollLeft = Instantiate<Gx::Button>("IDC_BUTTON_ITEM_SCROLL_LEFT");
    shopScrollLeft->SetClickCallback([=] (auto&, auto&) { shopScrollBar->Decrease(); });

    const auto shopScrollRight = Instantiate<Gx::Button>("IDC_BUTTON_ITEM_SCROLL_RIGHT");
    shopScrollRight->SetClickCallback([=] (auto&, auto&) { shopScrollBar->Increase(); });

    const auto shopItemList = Instantiate<Gx::List>("IDC_LIST_ITEM");
    shopItemList->SetScrollWheelCallback([=] (auto&, auto& ev)
    {
        if (ev.Delta > 0)
            shopScrollRight->PerformClick();
        else
            shopScrollLeft->PerformClick();
    });

    const auto setItemContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_SET_ITEM");
    const auto setItemList      = setItemContainer->FindChild<Gx::List>("IDC_LIST_SET_ITEM");
    setItemList->SetScrollWheelCallback([=] (auto&, auto& ev)
    {
        if (ev.Delta > 0)
            shopScrollRight->PerformClick();
        else
            shopScrollLeft->PerformClick();
    });

    const auto myBagContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_MYBAG");
    m_myBagSelectIndicator = myBagContainer->FindChild<Gx::Image>("IDC_IMAGE_MYBAG_SELECT");
    m_myBagSelectIndicator->SetVisible(false);

    const auto bagList = myBagContainer->FindChild<Gx::List>("IDC_LIST_BAG");
    const auto bagSlots = bagList->GetChildren();

    m_myBagCurrentPage = 0;
    const auto bagScrollBar = myBagContainer->FindChild<Gx::ScrollBar>("IDC_SCROLL_MYBAG");
    bagScrollBar->SetMaximumValue(std::ceil(static_cast<float>(m_inventory.size()) / 2.f));
    bagScrollBar->SetValueChangedCallback([this, sfxPrev, sfxNext] (auto&, const float value)
    {
        if (value < m_myBagCurrentPage)
            m_mixer.Play(sfxPrev, "SFX");
        else
            m_mixer.Play(sfxNext, "SFX");

        m_myBagCurrentPage = static_cast<unsigned int>(value);
        InvalidateMyBag();
    });

    const auto bagScrollLeftButton = myBagContainer->FindChild<Gx::Button>("IDC_BUTTON_MYBAG_SCROLL_LEFT");
    bagScrollLeftButton->SetClickCallback([=] (auto&, auto&) { bagScrollBar->Decrease(); });

    const auto bagScrollRightButton = myBagContainer->FindChild<Gx::Button>("IDC_BUTTON_MYBAG_SCROLL_RIGHT");
    bagScrollRightButton->SetClickCallback([=] (auto&, auto&) { bagScrollBar->Increase(); });

    bagList->SetScrollWheelCallback([=] (auto&, auto& ev)
    {
        if (ev.Delta > 0)
            bagScrollRightButton->PerformClick();
        else
            bagScrollLeftButton->PerformClick();
    });

    const auto sellButton = myBagContainer->FindChild<Gx::Button>("IDC_BUTTON_SELL");
    sellButton->SetClickCallback([this] (auto&, auto&) { OnItemSellClicked(); });

    m_cartCurrentPage = 0;
    const auto cartContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_CART");

    const auto cartList           = cartContainer->FindChild<Gx::List>("IDC_LIST_CART");
    const auto cartPrevPageButton = cartContainer->FindChild<Gx::Button>("IDC_BUTTON_LEFT");
    const auto cartNextPageButton = cartContainer->FindChild<Gx::Button>("IDC_BUTTON_RIGHT");

    cartPrevPageButton->SetClickCallback([this] (auto&, auto&)
    {
        if (m_cartCurrentPage > 0)
        {
            m_cartCurrentPage--;
            InvalidateCart();
        }
    });

    cartNextPageButton->SetClickCallback([this] (auto&, auto&)
    {
        m_cartCurrentPage++;
        InvalidateCart();
    });

    cartList->SetScrollWheelCallback([=] (auto&, auto& ev)
    {
        if (ev.Delta > 0)
            cartNextPageButton->PerformClick();
        else
            cartPrevPageButton->PerformClick();
    });

    const auto myBagButton = Instantiate<Gx::Button>("IDC_BUTTON_MYBAG");
    const auto cartButton  = Instantiate<Gx::Button>("IDC_BUTTON_CART");

    myBagButton->SetClickCallback([=] (auto&, auto&)
    {
        myBagContainer->SetEnabled(true);
        myBagContainer->SetVisible(true);

        cartContainer->SetEnabled(false);
        cartContainer->SetVisible(false);
    });

    cartButton->SetClickCallback([=] (auto&, auto&)
    {
        myBagContainer->SetEnabled(false);
        myBagContainer->SetVisible(false);

        cartContainer->SetEnabled(true);
        cartContainer->SetVisible(true);
    });

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateRoom>();
    });

    myBagButton->PerformClick();
    InvalidateMyBag();
    InvalidateCart();
    InvalidateShopItemList(true);

    bgm->setLooping(true);
    m_mixer.Play(bgm, "BGM");
}

void StateItemShop::OnItemSellClicked()
{
    auto& player         = m_session.GetCurrentPlayer();
    const auto sfxAccept = Instantiate<sf::Sound>("bgEffect/02");
    const auto sfxCancel = Instantiate<sf::Sound>("bgEffect/03");

    if (!m_myBagSelectedItem)
    {
        ShowDialog("No selected item.", DialogStyle::Information, false, [] (auto) {});
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
        ShowDialog("Selected item cannot be sold.", DialogStyle::Information, false, [] (auto) {});
        return;
    }

    const auto message = "Item: " + m_myBagSelectedItem->GetName() +
        "\nPrice: " + std::to_string(price) + " " + std::string(magic_enum::enum_name(currency)) +
        "\n\nAre you sure about selling the item?";

    ShowDialog(message, DialogStyle::OkCancel, false, [=, &player] (auto accepted)
    {
        if (!accepted)
        {
            m_mixer.Play(sfxCancel, "SFX");
            return;
        }

        player.Inventory.erase(
            std::remove_if(player.Inventory.begin(), player.Inventory.end(), [=] (auto i) { return i == m_myBagSelectedItem->GetID(); }),
            player.Inventory.end()
        );

        m_inventory.erase(
            std::remove_if(m_inventory.begin(), m_inventory.end(), [=] (auto i) { return i->GetID() == m_myBagSelectedItem->GetID(); }),
            m_inventory.end()
        );

        m_myBagSelectedItem = nullptr;
        if (currency == Currency::Gem)
            player.Gem += price;
        else
            player.Cash += price;

        m_mixer.Play(sfxAccept, "SFX");
        InvalidateMyBag();
    });
}

void StateItemShop::InvalidateMyBag()
{
    const auto& player   = m_session.GetCurrentPlayer();
    const auto avatar    = Instantiate<Avatar>("IDC_AVATAR");
    const auto sfxClick  = Instantiate<sf::Sound>("bgEffect/25");
    const auto container = Instantiate<Gx::UiContainer>("IDC_CONTAINER_MYBAG");
    const auto bagList   = container->FindChild<Gx::List>("IDC_LIST_BAG");
    const auto bagSlots  = bagList->GetChildren();

    if (m_inventory.empty())
    {
        for (const auto id : player.Inventory)
        {
            if (const auto item = m_items.GetItem(id); item)
                m_inventory.push_back(item);
        }
    }

    Gx::Image* currentSlot = nullptr;
    unsigned int itemCount = 0;
    auto inventory = std::vector<Item*>();
    for (auto item : m_inventory)
    {
        if (!avatar->IsEquiped(item))
            inventory.push_back(item);
    }

    for (std::size_t i = 0, j = m_myBagCurrentPage * 2; i < bagSlots.size(); i++)
    {
        const auto slot = dynamic_cast<Gx::Image*>(bagSlots[i]);
        if (!slot)
            continue;

        slot->SetClickCallback(nullptr);
        slot->SetDoubleClickCallback(nullptr);
        if (j >= inventory.size())
        {
            slot->SetVisible(false);
            continue;
        }

        const auto item = inventory[j++];
        itemCount++;

        currentSlot = item == m_myBagSelectedItem ? slot : currentSlot;
        if (item->GetType() == EquipmentType::AttributiveItem && item->GetLargeThumbnail() && item->GetLargeThumbnail()->GetTexture())
            slot->SetTexture(*item->GetLargeThumbnail()->GetTexture(), true);
        else if (item->GetSmallThumbnail() && item->GetSmallThumbnail()->GetTexture())
            slot->SetTexture(*item->GetSmallThumbnail()->GetTexture(), true);

        slot->SetVisible(true);
        slot->SetClickCallback([=] (auto&, auto&)
        {
            if (m_myBagSelectedItem == item)
                return;

            m_myBagSelectedItem = item;
            m_myBagSelectIndicator->SetVisible(true);

            slot->AddChild(*m_myBagSelectIndicator);
            m_mixer.Play(sfxClick, "SFX");
        });
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

    const auto bagScrollBar = container->FindChild<Gx::ScrollBar>("IDC_SCROLL_MYBAG");
    bagScrollBar->SetMaximumValue(std::ceil(static_cast<float>(inventory.size()) / 2.f));

    const auto currentGem = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_GEM");
    currentGem->SetValue(player.Gem);

    const auto currentCash = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_CASH");
    currentCash->SetValue(player.Cash);
}

void StateItemShop::InvalidateCart()
{
    const auto container = Instantiate<Gx::UiContainer>("IDC_CONTAINER_CART");
    const auto cartList  = container->FindChild<Gx::List>("IDC_LIST_CART");
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
            const auto& sets = setInfoData.Require->Sets.value();
            if (auto set = sets.find(item.ID); set != sets.end())
            {
                for (const auto itemID : set->second.Require->Items.value())
                {
                    calculateItem(itemID);

                    if (!set->second.Attributes->Discounts.has_value())
                        continue;

                    auto discounts = set->second.Attributes->Discounts->find(itemID);
                    if (discounts == set->second.Attributes->Discounts->end())
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
        const auto id           = slot->FindChild<Gx::Label>("IDC_TEXT_NUMBER");
        const auto name         = slot->FindChild<Gx::Label>("IDC_TEXT_NAME");
        const auto type         = slot->FindChild<Gx::Image>("IDC_IMAGE_ITEM_TYPE");
        const auto price        = slot->FindChild<Gx::Label>("IDC_TEXT_PRICE");
        const auto deleteButton = slot->FindChild<Gx::Button>("IDC_BUTTON_DELETE");

        id->SetString(std::to_string(j));
        if (item.Type == CartItemType::EquipmentSet)
        {
            const auto& sets = setInfoData.Require->Sets.value();
            if (auto set = sets.find(item.ID); set != sets.end())
            {
                name->SetString(sf::String::fromUtf8(set->second.Attributes->Name->begin(), set->second.Attributes->Name->end()));
                unsigned int setPriceGem  = 0;
                unsigned int setPriceCash = 0;

                for (const auto itemID : set->second.Require->Items.value())
                {
                    if (auto it = itemData.Items.find(itemID); it != itemData.Items.end())
                    {
                        auto metadata = it->second;
                        if (auto gemPrice = metadata.Prices.find(Currency::Gem); gemPrice != metadata.Prices.end())
                            setPriceGem += gemPrice->second;
                        else if (auto cashPrice = metadata.Prices.find(Currency::Cash); cashPrice != metadata.Prices.end())
                            setPriceCash += cashPrice->second;

                        if (!set->second.Attributes->Discounts.has_value())
                            continue;

                        auto discounts = set->second.Attributes->Discounts->find(itemID);
                        if (discounts == set->second.Attributes->Discounts->end())
                            continue;

                        if (auto gemDiscount = discounts->second.find(Currency::Gem); gemDiscount != discounts->second.end())
                            setPriceGem = setPriceGem < gemDiscount->second ? 0 : setPriceGem - gemDiscount->second;
                        else if (auto cashDiscount = discounts->second.find(Currency::Cash); cashDiscount != discounts->second.end())
                            setPriceCash = setPriceCash < cashDiscount->second ? 0 : setPriceCash - cashDiscount->second;
                    }
                }

                if (setPriceGem > 0)
                    price->SetString(std::to_string(setPriceGem) + "G");
                else
                    price->SetString(std::to_string(setPriceCash) + "M");
            }

            type->SetFrame("EquipmentSet");
        }
        else if (item.Type == CartItemType::Equipment)
        {
            if (auto it = itemData.Items.find(item.ID); it != itemData.Items.end())
            {
                auto metadata = it->second;
                name->SetString(metadata.Name);
                if (auto gemPrice = metadata.Prices.find(Currency::Gem); gemPrice != metadata.Prices.end())
                    price->SetString(std::to_string(gemPrice->second) + "G");
                else if (auto cashPrice = metadata.Prices.find(Currency::Cash); cashPrice != metadata.Prices.end())
                    price->SetString(std::to_string(cashPrice->second) + "M");
            }

            type->SetFrame("Equipment");
        }
        else if (item.Type == CartItemType::Music)
        {
            type->SetFrame("Music");
        }

        deleteButton->SetClickCallback([this, index = j - 1] (auto&, auto&)
        {
            m_cart.Remove(index);
            InvalidateCart();
        });
    }

    const auto currentPage = container->FindChild<Gx::BitmapNumber>("IDC_NUMBER_CURRENT_PAGE");
    const auto totalPage   = container->FindChild<Gx::BitmapNumber>("IDC_NUMBER_MAX_PAGE");
    const auto totalGem    = container->FindChild<Gx::BitmapNumber>("IDC_NUMBER_TOTAL_GEM");
    const auto totalCash   = container->FindChild<Gx::BitmapNumber>("IDC_NUMBER_TOTAL_CASH");

    currentPage->SetValue(maxPage > 0 ? m_cartCurrentPage + 1 : 0);
    totalPage->SetValue(maxPage);
    totalGem->SetValue(gem);
    totalCash->SetValue(cash);
}

void StateItemShop::InvalidateShopItemList(const bool rebuildList)
{
    const auto avatar        = Instantiate<Avatar>("IDC_AVATAR");
    const auto planet        = Instantiate<Gx::Image>("IDC_IMAGE_PLANET");
    const auto itemList      = Instantiate<Gx::List>("IDC_LIST_ITEM");
    const auto slots         = itemList->GetChildren();
    const auto shopScrollBar = Instantiate<Gx::ScrollBar>("IDC_SCROLL_ITEM");

    Instantiate<Gx::Image>("IDC_IMAGE_TOOLTIP")->SetVisible(false);
    if (m_itemCategory == EquipmentType::Costume)
    {
        InvalidateShopSetItemList(rebuildList);
        return;
    }

    const auto setItemContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_SET_ITEM");
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
        for (auto& [_, header] : m_items.GetItemData().Items)
        {
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

        shopScrollBar->SetMaximumValue(std::max(std::ceil(static_cast<float>(m_shopItemList.size()) / static_cast<float>(slots.size())) - 1.f, 0.f));
        if (shopScrollBar->GetValue() != 0)
        {
            shopScrollBar->SetValue(0); // This must trigger invalidate;
            return;
        }
    }

    for (std::size_t i = 0, j = m_shopCurrentPage * slots.size(); i < slots.size(); i++)
    {
        const auto slot = dynamic_cast<Gx::UiContainer*>(slots[i]);
        if (!slot)
            continue;

        if (j >= m_shopItemList.size())
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

        const auto name          = slot->FindChild<Gx::Label>("IDC_TEXT_NAME");
        const auto priceTag      = slot->FindChild<Gx::BitmapNumber>("IDC_NUMBER_PRICE");
        const auto currencyTag   = slot->FindChild<Gx::Image>("IDC_IMAGE_CURRENCY");
        const auto addButton     = slot->FindChild<Gx::Button>("IDC_BUTTON_ADD");
        const auto previewButton = slot->FindChild<Gx::Button>("IDC_BUTTON_PREVIEW");
        const auto thumbnail     = slot->FindChild<Gx::Image>("IDC_IMAGE_ITEM");

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

        addButton->SetClickCallback([this, metadata] (auto&, auto&)
        {
            const auto cartButton = Instantiate<Gx::Button>("IDC_BUTTON_CART");
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
        });

        previewButton->SetClickCallback([=, id = metadata.ID] (auto&, auto&)
        {
            avatar->Equip(m_items.GetItem(id));
        });

        thumbnail->SetVisible(false);
        thumbnail->SetFocusChangedCallback(nullptr);

        if (m_thumbnails.find(metadata.ID) == m_thumbnails.end())
            m_thumbnails[metadata.ID] = std::move(m_items.Create(metadata.ID));

        const auto item = m_thumbnails[metadata.ID].get();
        if (!item)
            continue;

        const auto sprite = item->GetLargeThumbnail();
        if (!sprite || !sprite->GetTexture())
            continue;

        thumbnail->SetVisible(true);
        thumbnail->SetTexture(*sprite->GetTexture());
        thumbnail->SetTexCoords(sprite->GetTexCoords());
        thumbnail->SetOrigin({
            thumbnail->GetTexCoords().size.x / 2.f,
            thumbnail->GetTexCoords().size.y / 2.f,
        });

        thumbnail->SetFocusChangedCallback([=, description = metadata.Description] (auto& sender, auto&)
        {
            const auto tooltip = Instantiate<Gx::Image>("IDC_IMAGE_TOOLTIP");
            Stop(m_tooltipDelay);
            if (sender.IsFocused() && m_tooltipDelay.GetState() != Gx::TaskState::Running && m_tooltipDelay.GetState() != Gx::TaskState::Completed)
            {
                m_tooltipDelay = Gx::Delay(sf::milliseconds(500), [=] ()
                {
                    const auto message = tooltip->FindChild<Gx::Label>("IDC_TEXT_MESSAGE");
                    message->SetString(description);

                    const auto  bounds = tooltip->GetGlobalBounds();
                    const float right  = bounds.position.x + bounds.size.x;

                    auto string = message->GetString();
                    std::size_t checkpoint = 0;
                    for (std::size_t c = 0; c < string.getSize(); c++)
                    {
                        if (string[c] == '\n')
                            continue;

                        if (string[c] == ' ')
                        {
                            checkpoint = c;
                            continue;
                        }

                        const auto position = message->FindCharacterPosition(c);
                        if (bounds.position.x + position.x > right - message->GetPosition().x)
                        {
                            string.replace(checkpoint, 1, "\n");
                            message->SetString(string);

                            c = 0;
                        }
                    }

                    tooltip->SetVisible(slot->IsVisible());
                });

                Run(m_tooltipDelay);
            }
            else
            {
                if (!sender.IsFocused())
                    m_tooltipDelay.Reset();

                tooltip->SetVisible(false);
            }
        });

        if (thumbnail->IsFocused())
        {
            // Force re-focus
            thumbnail->SetFocus(false);
            thumbnail->SetFocus(true);
        }
    }
}

void StateItemShop::InvalidateShopSetItemList(bool rebuildList)
{
    const auto currentAvatar    = Instantiate<Avatar>("IDC_AVATAR");
    const auto planet           = Instantiate<Gx::Image>("IDC_IMAGE_PLANET");
    const auto itemList         = Instantiate<Gx::List>("IDC_LIST_ITEM");
    const auto shopScrollBar    = Instantiate<Gx::ScrollBar>("IDC_SCROLL_ITEM");
    const auto setItemContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_SET_ITEM");
    const auto setItemList      = setItemContainer->FindChild<Gx::List>("IDC_LIST_SET_ITEM");
    const auto slots            = setItemList->GetChildren();

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
        const auto sets = setInfoData.Require->Sets.value();
        for (auto set = sets.rbegin(); set != sets.rend(); ++set)
        {
            const auto& header = set->second;

            // Check item set list
            if (!header.Require.has_value() || !header.Require->Items.has_value() || !header.Attributes.has_value())
                continue;

            // Check Gender
            if (header.Attributes->Gender != m_genderCategory && header.Attributes->Gender != Gender::Any)
                continue;

            // Check Planet
            if (m_shopPlanetCategory != Planet::Unknown && header.Attributes->Origin != Planet::Unknown && m_shopPlanetCategory != header.Attributes->Origin)
                continue;

            // Calculate prices
            auto setItems = std::vector<ItemMetadata>();
            auto prices   = std::unordered_map<Currency, unsigned int>();
            auto priceEnabled = std::unordered_map<Currency, bool>{ { Currency::Gem, true}, {Currency::Cash, true} };
            for (unsigned int itemID : header.Require->Items.value())
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


            m_shopSetList.push_back(header.Attributes.value());
            m_shopSetItemList[*header.Attributes->ID] = setItems;
            m_shopSetItemPrices[*header.Attributes->ID] = prices;
        }

        shopScrollBar->SetMaximumValue(std::max(std::ceil(static_cast<float>(m_shopSetItemList.size()) / static_cast<float>(slots.size())) - 1.f, 0.f));
        if (shopScrollBar->GetValue() != 0)
        {
            shopScrollBar->SetValue(0); // This must trigger invalidate;
            return;
        }
    }

    for (std::size_t i = 0, j = m_shopCurrentPage * slots.size(); i < slots.size(); i++)
    {
        const auto slot = dynamic_cast<Gx::UiContainer*>(slots[i]);
        if (!slot)
            continue;

        if (j >= m_shopSetItemList.size())
        {
            slot->SetEnabled(false);
            slot->SetVisible(false);
            continue;
        }

        auto metadata = m_shopSetList[j++];
        const auto itemSetList = m_shopSetItemList[*metadata.ID];

        auto currency = Currency::Gem;
        auto price    = 0;
        for (auto cur : { Currency::Gem, Currency::Cash })
        {
            price    = m_shopSetItemPrices[*metadata.ID][cur];
            currency = cur;
            if (price > 0)
                break;
        }

        slot->SetEnabled(true);
        slot->SetVisible(true);

        const auto name          = slot->FindChild<Gx::Label>("IDC_TEXT_NAME");
        const auto avatar        = slot->FindChild<Avatar>("IDC_AVATAR");
        const auto thumbnail     = slot->FindChild<Gx::Image>("IDC_IMAGE_ITEM");
        const auto pieceList     = slot->FindChild<Gx::List>("IDC_LIST_ITEM_PIECE");
        const auto priceTag      = slot->FindChild<Gx::BitmapNumber>("IDC_NUMBER_PRICE");
        const auto currencyTag   = slot->FindChild<Gx::Image>("IDC_IMAGE_CURRENCY");
        const auto addButton     = slot->FindChild<Gx::Button>("IDC_BUTTON_ADD");
        const auto previewButton = slot->FindChild<Gx::Button>("IDC_BUTTON_PREVIEW");

        name->SetString(sf::String::fromUtf8(metadata.Name->begin(), metadata.Name->end()));
        avatar->SetGender(m_genderCategory);

        for (auto [_, item] : m_items.GetDefaultItems(avatar->GetGender()))
            avatar->SetDefaultItem(item);

        avatar->ClearEquipments();
        for (const auto& itemMetadata : itemSetList)
            avatar->Equip(m_items.GetItem(itemMetadata.ID));

        priceTag->SetValue(price);

        if (currency == Currency::Gem)
            currencyTag->SetFrame("Gem");
        else
            currencyTag->SetFrame("Cash");

        currencyTag->SetPosition({
            priceTag->GetPosition().x - priceTag->GetLocalBounds().size.x - currencyTag->GetLocalBounds().size.x - 1,
            currencyTag->GetPosition().y
        });

        addButton->SetClickCallback([this, metadata] (auto&, auto&)
        {
            const auto cartButton = Instantiate<Gx::Button>("IDC_BUTTON_CART");
            if (m_cart.AddEquipmentSet(*metadata.ID))
            {
                m_cartCurrentPage = std::numeric_limits<std::uint32_t>::max();
                InvalidateCart();
            }

            cartButton->PerformClick();
        });

        previewButton->SetClickCallback([=, id = *metadata.ID] (auto&, auto&)
        {
            currentAvatar->ClearEquipments();
            for (const auto& itemMetadata : m_shopSetItemList[id])
                currentAvatar->Equip(m_items.GetItem(itemMetadata.ID));
        });

        const auto pieces = pieceList->GetChildren();
        for (std::size_t p = 0; p < pieces.size(); p++)
        {
            const auto pieceName = dynamic_cast<Gx::Label*>(pieces[p]);
            if (!pieceName)
                continue;

            pieceName->SetVisible(true);
            pieceName->SetString(std::to_string(p + 1));
            if (p < itemSetList.size())
                pieceName->SetString(pieceName->GetString() + " " + itemSetList[p].Name);
        }

        thumbnail->SetVisible(false);
        thumbnail->SetFocusChangedCallback([=, description = metadata.Description.value_or(std::string())] (auto& sender, auto&)
        {
            const auto tooltip = Instantiate<Gx::Image>("IDC_IMAGE_TOOLTIP");
            Stop(m_tooltipDelay);
            if (sender.IsFocused() && m_tooltipDelay.GetState() != Gx::TaskState::Running && m_tooltipDelay.GetState() != Gx::TaskState::Completed)
            {
                m_tooltipDelay = Gx::Delay(sf::milliseconds(500), [=] ()
                {
                    const auto message = tooltip->FindChild<Gx::Label>("IDC_TEXT_MESSAGE");
                    message->SetString(sf::String::fromUtf8(description.begin(), description.end()));

                    const auto  bounds = tooltip->GetGlobalBounds();
                    const float right  = bounds.position.x + bounds.size.x;

                    auto string = message->GetString();
                    std::size_t checkpoint = 0;
                    for (std::size_t c = 0; c < string.getSize(); c++)
                    {
                        if (string[c] == '\n')
                            continue;

                        if (string[c] == ' ')
                        {
                            checkpoint = c;
                            continue;
                        }

                        const auto position = message->FindCharacterPosition(c);
                        if (bounds.position.x + position.x > right - message->GetPosition().x)
                        {
                            string.replace(checkpoint, 1, "\n");
                            message->SetString(string);

                            c = 0;
                        }
                    }

                    tooltip->SetVisible(slot->IsVisible());
                });

                Run(m_tooltipDelay);
            }
            else
            {
                if (!sender.IsFocused())
                    m_tooltipDelay.Reset();

                tooltip->SetVisible(false);
            }
        });

        if (thumbnail->IsFocused())
        {
            // Force re-focus
            thumbnail->SetFocus(false);
            thumbnail->SetFocus(true);
        }
    }
}
