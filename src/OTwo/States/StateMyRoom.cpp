#include <magic_enum.hpp>
#include <OTwo/States/StateMyRoom.hpp>
#include <OTwo/States/StateRoom.hpp>

#include <OTwo/Avatar/Avatar.hpp>

#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/Contexts/SessionContext.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/Number.hpp>
#include <Genode/UI/ScrollBar.hpp>

StateMyRoom::StateMyRoom(State &&state) :
    State::State(std::move(state)),
    m_selectedItem(nullptr),
    m_bagSelect(nullptr)
{
}

void StateMyRoom::Initialize()
{
    auto& mixer       = Require<Gx::Mixer>();
    auto& session     = Require<SessionContext>();
    const auto& items = Require<ItemFactory>();
    auto& player      = session.GetCurrentPlayer();

    const auto bgm       = Instantiate<sf::Music>("BGM/bgMyroom.ogg");
    const auto sfxAccept = Instantiate<sf::Sound>("bgEffect/02");
    const auto sfxCancel = Instantiate<sf::Sound>("bgEffect/03");
    const auto sfxPrev   = Instantiate<sf::Sound>("bgEffect/19_1");
    const auto sfxNext   = Instantiate<sf::Sound>("bgEffect/19_2");

    const auto avatar = Instantiate<Avatar>("IDC_AVATAR");
    avatar->SetGender(player.Gender);
    for (auto [_, item] : items.GetDefaultItems(player.Gender))
        avatar->SetDefaultItem(item);

    for (const auto id : player.EquippedItemIDs)
    {
        if (const auto item = items.GetItem(id); item)
            avatar->Equip(item);
    }

    for (const auto id : player.Inventory)
    {
        if (const auto item = items.GetItem(id); item)
            m_inventory.push_back(item);
    }

    m_bagSelect = Instantiate<Gx::Image>("IDC_IMAGE_BAG_SELECT");
    m_bagSelect->SetVisible(false);

    const auto bagList = Instantiate<Gx::List>("IDC_LIST_BAG");
    const auto bagSlots = bagList->GetChildren();

    m_bagCurrentPage = 0;
    for (std::size_t i = 0; i < bagSlots.size(); i++)
    {
        const auto slot = dynamic_cast<Gx::Image*>(bagSlots[i]);
        if (!slot)
            continue;

        slot->SetDoubleClickCallback(nullptr);
    }

    const auto bagScrollBar = Instantiate<Gx::ScrollBar>("IDC_SCROLL_MY_BAG");
    bagScrollBar->SetMaximumValue(std::ceil(static_cast<float>(m_inventory.size()) / 2.f));
    bagScrollBar->SetValueChangedCallback([this, sfxPrev, sfxNext, &mixer] (auto&, const float value)
    {
        if (value < m_bagCurrentPage)
            mixer.Play(sfxPrev, "SFX");
        else
            mixer.Play(sfxNext, "SFX");

        m_bagCurrentPage = static_cast<unsigned int>(value);
        Invalidate();
    });

    const auto bagScrollLeft = Instantiate<Gx::Button>("IDC_BUTTON_SCROLL_LEFT");
    bagScrollLeft->SetClickCallback([=] (auto&, auto&) { bagScrollBar->Decrease(); });

    const auto bagScrollRight = Instantiate<Gx::Button>("IDC_BUTTON_SCROLL_RIGHT");
    bagScrollRight->SetClickCallback([=] (auto&, auto&) { bagScrollBar->Increase(); });

    bagList->SetScrollWheelCallback([=] (auto&, auto& ev) {
        if (ev.Delta > 0)
            bagScrollRight->PerformClick();
        else
            bagScrollLeft->PerformClick();
    });

    const auto equipmentsContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_EQUIPMENTS");
    equipmentsContainer->SetVisible(true);

    const auto gemNumber = Instantiate<Gx::Number>("IDC_NUMBER_GEM");
    gemNumber->SetValue(player.Gem);

    const auto statusPanel = Instantiate<Gx::Image>("IDC_IMAGE_STATUS");
    statusPanel->SetVisible(false);

    const auto nickname = statusPanel->FindChild<Gx::Label>("IDC_TEXT_NAME");
    const auto level    = statusPanel->FindChild<Gx::Label>("IDC_TEXT_LEVEL");
    const auto epoint   = statusPanel->FindChild<Gx::Label>("IDC_TEXT_EVENT_POINT");
    const auto exp      = statusPanel->FindChild<Gx::Label>("IDC_TEXT_EXP");
    const auto nextExp  = statusPanel->FindChild<Gx::Label>("IDC_TEXT_NEXT_EXP");
    const auto record   = statusPanel->FindChild<Gx::Label>("IDC_TEXT_RECORD");
    const auto ranking  = statusPanel->FindChild<Gx::Label>("IDC_TEXT_RANKING");
    const auto guild    = statusPanel->FindChild<Gx::Label>("IDC_TEXT_GUILD");

    nickname->SetString(player.Name);
    level->SetString(std::to_string(player.Level));
    epoint->SetString(std::to_string(player.EventPoint));
    exp->SetString(std::to_string(player.Exp));
    nextExp->SetString(std::to_string(player.NextExp));
    record->SetString("Wins: " + std::to_string(player.Wins) + " / Draws: " + std::to_string(player.Draws) + " / Loses: " + std::to_string(player.Loses));
    ranking->SetString(std::to_string(player.Rank));
    guild->SetString(player.Guild.Name);

    const auto albumButton = statusPanel->FindChild<Gx::Button>("IDC_BUTTON_MY_ALBUM");
    albumButton->SetClickCallback([=, &mixer] (auto&, auto&)
    {
        ShowDialog("Album mode is currently not available", DialogStyle::Information, false, [=, &mixer] (auto) { mixer.Play(sfxAccept); });
    });

    const auto sellButton = Instantiate<Gx::Button>("IDC_BUTTON_SELL");
    sellButton->SetClickCallback([=, &player, &mixer] (auto&, auto&)
    {
        if (statusPanel->IsVisible())
            return;

        if (!m_selectedItem)
        {
            ShowDialog("No selected item.", DialogStyle::Information, false, [] (auto) {});
            return;
        }

        auto currency = Currency::Gem;
        auto price    = 0;

        for (auto cur : { Currency::Gem, Currency::MCash })
        {
            price    = m_selectedItem->GetPrice(cur);
            currency = cur;
            if (price > 0)
                break;
        }

        if (price <= 0)
        {
            ShowDialog("Selected item cannot be sold.", DialogStyle::Information, false, [] (auto) {});
            return;
        }

        const auto message = "Item: " + m_selectedItem->GetName() +
            "\nPrice: " + std::to_string(price) + " " + std::string(magic_enum::enum_name(currency)) +
            "\n\nAre you sure about selling the item?";

        ShowDialog(message, DialogStyle::OkCancel, false, [=, &player, &mixer] (auto accepted)
        {
            if (!accepted)
            {
                mixer.Play(sfxCancel, "SFX");
                return;
            }

            player.Inventory.erase(
                std::remove_if(player.Inventory.begin(), player.Inventory.end(), [=] (auto i) { return i == m_selectedItem->GetID(); }),
                player.Inventory.end()
            );

            m_inventory.erase(
                std::remove_if(m_inventory.begin(), m_inventory.end(), [=] (auto i) { return i->GetID() == m_selectedItem->GetID(); }),
                m_inventory.end()
            );

            m_selectedItem = nullptr; m_selectedItem = nullptr;
            if (currency == Currency::Gem)
                player.Gem += price;
            else
                player.Cash += price;

            mixer.Play(sfxAccept, "SFX");
            Invalidate();
        });
    });

    const auto inventoryButton = Instantiate<Gx::CheckBox>("IDC_CHECKBOX_INVENTORY");
    inventoryButton->SetCheckStateChangeCallback([=] (auto checkBox)
    {
        statusPanel->SetVisible(checkBox->IsChecked());
        bagList->SetEnabled(!checkBox->IsChecked());
        equipmentsContainer->SetVisible(!checkBox->IsChecked());
        equipmentsContainer->SetEnabled(!checkBox->IsChecked());
    });

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateRoom>();
    });

    bgm->setLoop(true);
    mixer.Play(bgm, "BGM");

    Invalidate();
}

void StateMyRoom::Invalidate()
{
    auto& session        = Require<SessionContext>();
    auto& mixer          = Require<Gx::Mixer>();
    const auto player    = &session.GetCurrentPlayer();
    const auto avatar    = Instantiate<Avatar>("IDC_AVATAR");
    const auto container = Instantiate<Gx::UiContainer>("IDC_CONTAINER_EQUIPMENTS");
    const auto sfxClick  = Instantiate<sf::Sound>("bgEffect/25");

    const auto bagList  = Instantiate<Gx::List>("IDC_LIST_BAG");
    const auto bagSlots = bagList->GetChildren();

    Gx::Image* currentSlot = nullptr;
    unsigned int itemCount = 0;
    auto inventory = std::vector<Item*>();
    for (auto item : m_inventory)
    {
        if (!avatar->IsEquiped(item))
            inventory.push_back(item);
    }

    for (std::size_t i = 0, j = m_bagCurrentPage * 2; i < bagSlots.size(); i++)
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

        currentSlot = item == m_selectedItem ? slot : currentSlot;
        if (item->GetType() == EquipmentType::AttributiveItem && item->GetLargePreview() && item->GetLargePreview()->GetTexture())
            slot->SetTexture(*item->GetLargePreview()->GetTexture(), true);
        else if (item->GetSmallPreview() && item->GetSmallPreview()->GetTexture())
            slot->SetTexture(*item->GetSmallPreview()->GetTexture(), true);

        slot->SetVisible(true);
        slot->SetClickCallback([=, &mixer] (auto&, auto&)
        {
            if (m_selectedItem == item)
                return;

            m_selectedItem = item;
            m_bagSelect->SetVisible(true);

            slot->AddChild(m_bagSelect);
            mixer.Play(sfxClick, "SFX");
        });

        slot->SetDoubleClickCallback([=] (auto&, auto&)
        {
            if (avatar->IsEquiped(item))
                avatar->Unequip(item);
            else
                avatar->Equip(item);

            avatar->ResetRenderables();

            m_selectedItem = nullptr;
            player->EquippedItemIDs.clear();
            for (auto [_, item] : avatar->GetEquipedItems())
                player->EquippedItemIDs.push_back(item->GetID());

            Invalidate();
        });
    }

    if (!currentSlot)
    {
        if (m_bagSelect->GetParent())
            m_bagSelect->GetParent()->RemoveChild(m_bagSelect);

        m_bagSelect->SetVisible(false);
    }
    else
    {
        currentSlot->AddChild(m_bagSelect);
        m_bagSelect->SetVisible(true);
    }

    const auto equippedItems  = avatar->GetEquipedItems();
    const auto instrument = Instantiate<Gx::Image>("IDC_IMAGE_INSTRUMENT");
    instrument->SetVisible(false);
    instrument->SetDoubleClickCallback(nullptr);
    for (auto type : { EquipmentType::Keyboard, EquipmentType::Bass, EquipmentType::Drum, EquipmentType::Guitar })
    {
        if (auto it = equippedItems.find(type); it != equippedItems.end())
        {
            InvalidateSlot(instrument, type);
            break;
        }
    }

    if (const auto it = equippedItems.find(EquipmentType::Costume); it != equippedItems.end())
        InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_HAIR"), EquipmentType::Costume);
    else
        InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_HAIR"), EquipmentType::Hair);

    InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_HAIR_ACCESSORIES"),       EquipmentType::HairAccessories);
    InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_WINGS"),                  EquipmentType::Wings);
    InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_ACCESSORIES"),            EquipmentType::Accessories);
    InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_INSTRUMENT_ACCESSORIES"), EquipmentType::InstrumentAccessories);
    InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_PET"),                    EquipmentType::Pet);
    InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_TOP"),                    EquipmentType::Top);
    InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_PANTS"),                  EquipmentType::Pants);
    InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_SHOES"),                  EquipmentType::Shoes);
    InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_GLASSES"),                EquipmentType::Glasses);
    InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_EARRINGS"),               EquipmentType::Earrings);
    InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_NECKLACE"),               EquipmentType::Necklace);
    InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_GLOVES"),                 EquipmentType::Gloves);
    InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_CLOTHES_ACCESSORIES"),    EquipmentType::ClothesAccessories);

    const auto bagScrollBar = Instantiate<Gx::ScrollBar>("IDC_SCROLL_MY_BAG");
    bagScrollBar->SetMaximumValue(std::ceil(static_cast<float>(inventory.size()) / 2.f));

    const auto gemNumber = Instantiate<Gx::Number>("IDC_NUMBER_GEM");
    gemNumber->SetValue(player->Gem);
}

void StateMyRoom::InvalidateSlot(Gx::Image* slot, const EquipmentType type, RenderPart preview)
{
    if (!slot)
        return;

    if (preview != RenderPart::LargePreview && preview != RenderPart::SmallPreview)
        preview = RenderPart::LargePreview;

    auto& session       = Require<SessionContext>();
    auto& mixer         = Require<Gx::Mixer>();
    const auto player   = &session.GetCurrentPlayer();
    const auto sfxDress = Instantiate<sf::Sound>("bgEffect/27_dress");

    const auto avatar        = Instantiate<Avatar>("IDC_AVATAR");
    const auto equippedItems = avatar->GetEquipedItems();

    slot->SetVisible(false);
    slot->SetDoubleClickCallback(nullptr);

    if (const auto it = equippedItems.find(type); it != equippedItems.end())
    {
        const auto item = it->second;
        slot->SetVisible(true);
        if (preview == RenderPart::LargePreview)
            slot->SetTexture(*it->second->GetLargePreview()->GetTexture(), true);
        else
            slot->SetTexture(*it->second->GetSmallPreview()->GetTexture(), true);

        slot->SetDoubleClickCallback([=, &mixer] (auto&, auto&)
        {
            avatar->Unequip(item);
            player->EquippedItemIDs.clear();
            for (auto [_, item] : avatar->GetEquipedItems())
                player->EquippedItemIDs.push_back(item->GetID());

            mixer.Play(sfxDress, "SFX");
            Invalidate();
        });
    }
}
