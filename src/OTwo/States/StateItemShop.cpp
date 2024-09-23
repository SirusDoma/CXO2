#include <emmintrin.h>
#include <future>
#include <OTwo/States/StateItemShop.hpp>
#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StateMyRoom.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/ScrollBar.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/RadioButton.hpp>

#include <magic_enum.hpp>
#include <mutex>

StateItemShop::StateItemShop(Gx::Mixer& mixer, SessionContext& session, ItemFactory& items) :
    m_mixer(mixer),
    m_session(session),
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
    const auto sfxPrev = Instantiate<sf::Sound>("bgEffect/19_1");
    const auto sfxNext = Instantiate<sf::Sound>("bgEffect/19_2");

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

    const auto gemNumber = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_GEM");
    gemNumber->SetValue(player.Gem);

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
        button->SetCheckStateChangeCallback([this, category, shopCategoryContainerMap, itemCategoryMap] (auto& sender)
        {
            if (!sender.IsChecked())
                return;

            m_shopCategory = category;
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
                radio->SetCheckStateChangeCallback([this, i, category, itemCategoryMap] (auto& sender)
                {
                    if (!sender.IsChecked())
                        return;

                    m_itemCategory = itemCategoryMap.at(category).at(i);
                    InvalidateShopItemList(true);
                });
            }
        }
    }

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
    });

    femaleButton->SetClickCallback([=] (auto&, auto&)
    {
        m_genderCategory = m_genderCategory = Gender::Male;

        femaleButton->SetEnabled(false);
        femaleButton->SetVisible(false);

        maleButton->SetEnabled(true);
        maleButton->SetVisible(true);
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

    const auto bagScrollLeft = myBagContainer->FindChild<Gx::Button>("IDC_BUTTON_MYBAG_SCROLL_LEFT");
    bagScrollLeft->SetClickCallback([=] (auto&, auto&) { bagScrollBar->Decrease(); });

    const auto bagScrollRight = myBagContainer->FindChild<Gx::Button>("IDC_BUTTON_MYBAG_SCROLL_RIGHT");
    bagScrollRight->SetClickCallback([=] (auto&, auto&) { bagScrollBar->Increase(); });

    bagList->SetScrollWheelCallback([=] (auto&, auto& ev)
    {
        if (ev.Delta > 0)
            bagScrollRight->PerformClick();
        else
            bagScrollLeft->PerformClick();
    });

    const auto sellButton = myBagContainer->FindChild<Gx::Button>("IDC_BUTTON_SELL");
    sellButton->SetClickCallback([this] (auto&, auto&) { OnItemSellClicked(); });

    const auto cartContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_CART");

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

    for (auto cur : { Currency::Gem, Currency::MCash })
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

    const auto gemNumber = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_GEM");
    gemNumber->SetValue(player.Gem);
}

void StateItemShop::InvalidateShopItemList(const bool rebuildList)
{
    const auto avatar        = Instantiate<Avatar>("IDC_AVATAR");
    const auto itemList      = Instantiate<Gx::List>("IDC_LIST_ITEM");
    const auto slots         = itemList->GetChildren();
    const auto shopScrollBar = Instantiate<Gx::ScrollBar>("IDC_SCROLL_ITEM");

    Instantiate<Gx::Image>("IDC_IMAGE_TOOLTIP")->SetVisible(false);
    if (rebuildList)
    {
        m_shopItemList.clear();
        for (auto& [_, header] : m_items.GetItemData().Items)
        {
            // Check price
            if (header.Prices.find(Currency::Gem) == header.Prices.end() && header.Prices.find(Currency::MCash) == header.Prices.end())
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

    static std::array<std::mutex,  100> mutexes;

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
        for (auto cur : { Currency::Gem, Currency::MCash })
        {
            price    = metadata.Prices[cur];
            currency = cur;
            if (price > 0)
                break;
        }

        slot->SetEnabled(true);
        slot->SetVisible(true);

        const auto name = slot->FindChild<Gx::Label>("IDC_TEXT_NAME");
        name->SetString(metadata.Name);

        const auto priceTag = slot->FindChild<Gx::BitmapNumber>("IDC_NUMBER_PRICE");
        priceTag->SetValue(price);

        const auto currencyTag = slot->FindChild<Gx::Image>("IDC_IMAGE_CURRENCY");
        if (currency == Currency::Gem)
            currencyTag->SetFrame("Gem");
        else
            currencyTag->SetFrame("Cash");

        currencyTag->SetPosition({
            priceTag->GetPosition().x - priceTag->GetLocalBounds().size.x - currencyTag->GetLocalBounds().size.x - 1,
            currencyTag->GetPosition().y
        });

        const auto previewButton = slot->FindChild<Gx::Button>("IDC_BUTTON_PREVIEW");
        previewButton->SetClickCallback([=, id = metadata.ID] (auto&, auto&)
        {
            avatar->Equip(m_items.GetItem(id));
        });

        const auto thumbnail = slot->FindChild<Gx::Image>("IDC_IMAGE_ITEM");
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
