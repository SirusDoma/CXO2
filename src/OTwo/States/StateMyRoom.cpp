#include <OTwo/States/StateMyRoom.hpp>
#include <OTwo/States/StateItemShop.hpp>
#include <OTwo/States/StateRoom.hpp>

#include <OTwo/Avatar/Avatar.hpp>

#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/Contexts/SessionContext.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/ScrollBar.hpp>

#include <magic_enum.hpp>

StateMyRoom::StateMyRoom(Gx::Mixer& mixer, SessionContext& session, ItemFactory& items) :
    m_mixer(mixer),
    m_session(session),
    m_items(items),
    m_selectedItem(nullptr),
    m_bagSelectIndicator(nullptr)
{
}

void StateMyRoom::Initialize()
{
    State::Initialize();
    
    auto& player         = m_session.GetCurrentPlayer();
    const auto bgm       = Instantiate<sf::Music>("BGM/bgMyroom.ogg");
    const auto sfxAccept = Instantiate<sf::Sound>("bgEffect/02");
    const auto sfxCancel = Instantiate<sf::Sound>("bgEffect/03");
    const auto sfxPrev   = Instantiate<sf::Sound>("bgEffect/19_1");
    const auto sfxNext   = Instantiate<sf::Sound>("bgEffect/19_2");

    const auto avatar = Instantiate<Avatar>("IDC_AVATAR");
    avatar->SetGender(player.Gender);
    for (auto [_, item] : m_items.GetDefaultItems(player.Gender))
        avatar->SetDefaultItem(std::move(item));

    for (const auto id : player.EquippedItemIDs)
        avatar->Equip(m_items.Create(id));

    for (const auto id : player.Inventory)
    {
        if (const auto item = m_items.Create(id); item.GetID() != 0)
            m_inventory.push_back(std::move(item));
    }

    m_bagSelectIndicator = Instantiate<Gx::Image>("IDC_IMAGE_MYBAG_SELECT");
    m_bagSelectIndicator->SetVisible(false);

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

    const auto bagScrollBar = Instantiate<Gx::ScrollBar>("IDC_SCROLL_MYBAG");
    bagScrollBar->SetMaximumValue(((m_inventory.size() + bagList->GetVerticalCount() - 1) / bagList->GetVerticalCount()) - 1);
    bagScrollBar->SetValueChangedCallback([this, sfxPrev, sfxNext] (auto&, const float value)
    {
        if (value < m_bagCurrentPage)
            m_mixer.Play(sfxPrev, "SFX");
        else
            m_mixer.Play(sfxNext, "SFX");

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

    const auto currentGem = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_GEM");
    currentGem->SetValue(player.Gem);

    const auto currentCash = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_CASH");
    currentCash->SetValue(player.Cash);

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
    albumButton->SetClickCallback([=] (auto&, auto&)
    {
        ShowDialog("Album mode is currently not available", DialogStyle::Information, false, [=] (auto) { m_mixer.Play(sfxAccept); });
    });

    const auto sellButton = Instantiate<Gx::Button>("IDC_BUTTON_SELL");
    sellButton->SetClickCallback([=, &player] (auto&, auto&)
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

        for (auto cur : { Currency::Gem, Currency::Cash })
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

        ShowDialog(message, DialogStyle::OkCancel, false, [=, &player] (auto accepted)
        {
            if (!accepted)
            {
                m_mixer.Play(sfxCancel, "SFX");
                return;
            }

            player.Inventory.erase(
                std::remove_if(player.Inventory.begin(), player.Inventory.end(), [=] (auto i) { return i == m_selectedItem->GetID(); }),
                player.Inventory.end()
            );

            m_inventory.erase(
                std::remove_if(m_inventory.begin(), m_inventory.end(), [=] (auto i) { return i.GetID() == m_selectedItem->GetID(); }),
                m_inventory.end()
            );

            m_selectedItem = nullptr;
            if (currency == Currency::Gem)
                player.Gem += price;
            else
                player.Cash += price;

            m_mixer.Play(sfxAccept, "SFX");
            Invalidate();
        });
    });

    const auto inventoryButton = Instantiate<Gx::CheckBox>("IDC_CHECKBOX_INVENTORY");
    inventoryButton->SetCheckStateChangeCallback([=] (auto& sender)
    {
        statusPanel->SetVisible(sender.IsChecked());
        statusPanel->SetEnabled(sender.IsChecked());
        bagList->SetEnabled(!sender.IsChecked());
        equipmentsContainer->SetVisible(!sender.IsChecked());
        equipmentsContainer->SetEnabled(!sender.IsChecked());
    });

    const auto shopButton = Instantiate<Gx::Button>("IDC_BUTTON_SHOP");
    shopButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateItemShop>();
    });

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateRoom>();
    });

    bgm->setLooping(true);
    m_mixer.Play(bgm, "BGM");

    Invalidate();
}

void StateMyRoom::Invalidate()
{
    const auto player    = &m_session.GetCurrentPlayer();
    const auto avatar    = Instantiate<Avatar>("IDC_AVATAR");
    const auto container = Instantiate<Gx::UiContainer>("IDC_CONTAINER_EQUIPMENTS");
    const auto sfxClick  = Instantiate<sf::Sound>("bgEffect/25");

    const auto bagList  = Instantiate<Gx::List>("IDC_LIST_BAG");
    const auto bagSlots = bagList->GetChildren();

    Gx::Image* currentSlot = nullptr;
    unsigned int itemCount = 0;
    auto inventory = std::vector<Item*>();
    for (auto& item : m_inventory)
    {
        if (!avatar->IsEquiped(item))
            inventory.push_back(&item);
    }

    for (std::size_t i = 0, j = m_bagCurrentPage * bagList->GetVerticalCount(); i < bagSlots.size(); i++)
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
        if (item->GetType() == EquipmentType::AttributiveItem && item->GetLargeThumbnail().GetTexture())
            slot->SetTexture(*item->GetLargeThumbnail().GetTexture(), true);
        else if (item->GetSmallThumbnail().GetTexture())
            slot->SetTexture(*item->GetSmallThumbnail().GetTexture(), true);

        slot->SetVisible(true);
        slot->SetClickCallback([=] (auto&, auto&)
        {
            if (m_selectedItem == item)
                return;

            m_selectedItem = item;
            m_bagSelectIndicator->SetVisible(true);

            slot->AddChild(*m_bagSelectIndicator);
            m_mixer.Play(sfxClick, "SFX");
        });

        slot->SetDoubleClickCallback([=] (auto&, auto&)
        {
            if (avatar->IsEquiped(*item))
                avatar->Unequip(item->GetType());
            else
                avatar->Equip(*item);

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
        if (m_bagSelectIndicator->GetParent())
            m_bagSelectIndicator->GetParent()->RemoveChild(*m_bagSelectIndicator);

        m_bagSelectIndicator->SetVisible(false);
    }
    else
    {
        currentSlot->AddChild(*m_bagSelectIndicator);
        m_bagSelectIndicator->SetVisible(true);
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

    const auto bagScrollBar = Instantiate<Gx::ScrollBar>("IDC_SCROLL_MYBAG");
    bagScrollBar->SetMaximumValue(((inventory.size() + bagList->GetVerticalCount() - 1) / bagList->GetVerticalCount()) - 1);

    const auto currentGem = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_GEM");
    currentGem->SetValue(player->Gem);

    const auto currentCash = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_CASH");
    currentCash->SetValue(player->Cash);
}

void StateMyRoom::InvalidateSlot(Gx::Image* slot, const EquipmentType type, RenderPart thumbnailType)
{
    if (!slot)
        return;

    if (thumbnailType != RenderPart::SmallThumbnail)
        thumbnailType = RenderPart::LargeThumbnail;

    const auto player   = &m_session.GetCurrentPlayer();
    const auto sfxDress = Instantiate<sf::Sound>("bgEffect/27_dress");

    const auto avatar        = Instantiate<Avatar>("IDC_AVATAR");
    const auto equippedItems = avatar->GetEquipedItems();

    slot->SetVisible(false);
    slot->SetDoubleClickCallback(nullptr);

    if (const auto it = equippedItems.find(type); it != equippedItems.end())
    {
        const auto item = it->second;
        slot->SetVisible(true);
        if (thumbnailType == RenderPart::LargeThumbnail)
            slot->SetTexture(*it->second->GetLargeThumbnail().GetTexture(), true);
        else
            slot->SetTexture(*it->second->GetSmallThumbnail().GetTexture(), true);

        slot->SetDoubleClickCallback([=] (auto&, auto&)
        {
            avatar->Unequip(item->GetType());
            player->EquippedItemIDs.clear();
            for (auto [_, item] : avatar->GetEquipedItems())
                player->EquippedItemIDs.push_back(item->GetID());

            m_mixer.Play(sfxDress, "SFX");
            Invalidate();
        });
    }
}
