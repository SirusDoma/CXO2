#include <OTwo/States/StateMyRoom.hpp>
#include <OTwo/States/StateItemShop.hpp>
#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StatePlanet.hpp>

#include <OTwo/Avatar/Avatar.hpp>

#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/Contexts/SessionContext.hpp>

#include <OTwo/Network/Exception.hpp>
#include <OTwo/Services/CharacterService.hpp>
#include <OTwo/Services/ItemShopService.hpp>

#include <OTwo/Messages/Requests/EquipItemRequest.hpp>
#include <OTwo/Messages/Responses/EquipItemResponse.hpp>
#include <OTwo/Messages/Responses/SellItemResponse.hpp>

#include <OTwo/StringTable/Identifiers/Sound.hpp>
#include <OTwo/StringTable/Identifiers/MyRoom.hpp>
#include <OTwo/Utilities/StringFormatter.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/ToggleButton.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/ScrollBar.hpp>

#include <magic_enum/magic_enum.hpp>
#include <fmt/format.h>

using namespace StringTable::Identifiers;

namespace
{
    ItemEquipSlotType GetItemEquipSlotType(EquipmentType type)
    {
        switch (type)
        {
            case EquipmentType::Guitar:             return ItemEquipSlotType::Instrument;
            case EquipmentType::Bass:               return ItemEquipSlotType::Instrument;
            case EquipmentType::Keyboard:           return ItemEquipSlotType::Instrument;
            case EquipmentType::Drum:               return ItemEquipSlotType::Instrument;
            case EquipmentType::Hair:               return ItemEquipSlotType::Hair;
            case EquipmentType::Earrings:           return ItemEquipSlotType::Earrings;
            case EquipmentType::Gloves:             return ItemEquipSlotType::Gloves;
            case EquipmentType::Accessories:        return ItemEquipSlotType::Accessories;
            case EquipmentType::Top:                return ItemEquipSlotType::Top;
            case EquipmentType::Pants:              return ItemEquipSlotType::Pants;
            case EquipmentType::Glasses:            return ItemEquipSlotType::Glasses;
            case EquipmentType::Necklace:           return ItemEquipSlotType::Necklace;
            case EquipmentType::ClothesAccessories: return ItemEquipSlotType::ClothesAccessories;
            case EquipmentType::Shoes:              return ItemEquipSlotType::Shoes;
            case EquipmentType::Face:               return ItemEquipSlotType::Face;
            default:                                return ItemEquipSlotType{};
        }
    }
}

StateMyRoom::StateMyRoom(Gx::AudioMixer& mixer, SessionContext& session, CharacterService& service, ItemShopService& shopService, ItemFactory& items) :
    m_mixer(mixer),
    m_session(session),
    m_service(service),
    m_shopService(shopService),
    m_items(items),
    m_selectedItem(nullptr),
    m_bagSelectIndicator(nullptr)
{
}

void StateMyRoom::Initialize()
{
    State::Initialize();
    
    auto& charInfo       = m_session.GetCharacterInfo();
    const auto bgm       = Instantiate<sf::Music>(Sound::BGM::BG_MY_ROOM);
    const auto sfxAccept = Instantiate<sf::Sound>(Sound::Effects::EF_02);
    const auto sfxCancel = Instantiate<sf::Sound>(Sound::Effects::EF_03);
    const auto sfxPrev   = Instantiate<sf::Sound>(Sound::Effects::EF_19_1);
    const auto sfxNext   = Instantiate<sf::Sound>(Sound::Effects::EF_19_2);

    const auto avatar = Instantiate<Avatar>(Resource::MyRoom::IDC_AVATAR);
    avatar->SetGender(charInfo.Gender);
    for (auto [_, item] : m_items.GetDefaultItems(charInfo.Gender))
        avatar->SetDefaultItem(std::move(item));

    for (const auto id : charInfo.EquippedItemIDs)
        avatar->Equip(m_items.Create(id));

    for (const auto id : charInfo.Inventory)
    {
        const auto item = m_items.Create(id);
        m_inventory.push_back(std::move(item));
    }

    m_bagSelectIndicator = Instantiate<Gx::Image>(Resource::MyRoom::IDC_IMAGE_MYBAG_SELECT);
    m_bagSelectIndicator->SetVisible(false);

    const auto bagList = Instantiate<Gx::List>(Resource::MyRoom::IDC_LIST_BAG);
    const auto bagSlots = bagList->GetChildren();

    m_bagCurrentPage = 0;
    for (std::size_t i = 0; i < bagSlots.size(); i++)
    {
        const auto slot = dynamic_cast<Gx::UiContainer*>(bagSlots[i]);
        if (!slot)
            continue;

        slot->SetDoubleClickCallback(nullptr);
    }

    const auto scrollControls = Instantiate<Gx::UiContainer>(Resource::MyRoom::IDC_CONTAINER_BAG_SCROLL_CONTROLS);
    const auto bagScrollBar = scrollControls->FindChild<Gx::ScrollBar>(Resource::MyRoom::IDC_SCROLL_MYBAG);

    // TODO: Detect vertical count?
    constexpr unsigned int verticalCount = 2; //bagList->GetVerticalCount();
    bagScrollBar->SetMaximumValue(m_inventory.size() < bagSlots.size() ? 0 : static_cast<int>(std::ceil(static_cast<float>(m_inventory.size() - bagSlots.size()) / verticalCount)));
    bagScrollBar->SetValueChangedCallback([this, sfxPrev, sfxNext] (auto&, const float value)
    {
        if (value < m_bagCurrentPage)
            m_mixer.Play(*sfxPrev, Sound::Channel::SFX);
        else
            m_mixer.Play(*sfxNext, Sound::Channel::SFX);

        m_bagCurrentPage = static_cast<unsigned int>(value);
        Invalidate();
    });

    const auto bagScrollLeft = scrollControls->FindChild<Gx::Button>(Resource::MyRoom::IDC_BUTTON_SCROLL_LEFT);
    bagScrollLeft->SetClickCallback([=] (auto&, auto&) { bagScrollBar->Decrease(); });

    const auto bagScrollRight = scrollControls->FindChild<Gx::Button>(Resource::MyRoom::IDC_BUTTON_SCROLL_RIGHT);
    bagScrollRight->SetClickCallback([=] (auto&, auto&) { bagScrollBar->Increase(); });

    bagList->SetScrollWheelCallback([=] (auto&, auto& ev) {
        if (ev.Delta > 0)
            bagScrollRight->PerformClick();
        else
            bagScrollLeft->PerformClick();
    });

    const auto equipmentsContainer = Instantiate<Gx::UiContainer>(Resource::MyRoom::IDC_CONTAINER_EQUIPMENTS);
    equipmentsContainer->SetVisible(true);

    const auto currentGem = Instantiate<Gx::BitmapNumber>(Resource::MyRoom::IDC_NUMBER_GEM);
    currentGem->SetValue(charInfo.Wallet.Gem);

    const auto currentCash = Instantiate<Gx::BitmapNumber>(Resource::MyRoom::IDC_NUMBER_CASH);
    currentCash->SetValue(charInfo.Wallet.Cash);

    const auto statusPanel = Instantiate<Gx::Image>(Resource::MyRoom::IDC_IMAGE_STATUS);
    statusPanel->SetEnabled(false);
    statusPanel->SetVisible(false);

    const auto nickname = statusPanel->FindChild<Gx::Label>(Resource::MyRoom::Status::IDC_TEXT_NAME);
    const auto level    = statusPanel->FindChild<Gx::Label>(Resource::MyRoom::Status::IDC_TEXT_LEVEL);
    const auto epoint   = statusPanel->FindChild<Gx::Label>(Resource::MyRoom::Status::IDC_TEXT_EVENT_POINT);
    const auto exp      = statusPanel->FindChild<Gx::Label>(Resource::MyRoom::Status::IDC_TEXT_EXP);
    const auto nextExp  = statusPanel->FindChild<Gx::Label>(Resource::MyRoom::Status::IDC_TEXT_NEXT_EXP);
    const auto record   = statusPanel->FindChild<Gx::Label>(Resource::MyRoom::Status::IDC_TEXT_RECORD);
    const auto ranking  = statusPanel->FindChild<Gx::Label>(Resource::MyRoom::Status::IDC_TEXT_RANKING);
    const auto guild    = statusPanel->FindChild<Gx::Label>(Resource::MyRoom::Status::IDC_TEXT_GUILD);

    nickname->SetString(charInfo.Name);
    level->SetString(std::to_string(charInfo.Level));
    epoint->SetString(std::to_string(charInfo.Wallet.Cash));
    exp->SetString(std::to_string(charInfo.Experience));
    nextExp->SetString(std::to_string(0));
    record->SetString(fmt::format("Wins: {} / Draws: {} / Loses: {}", charInfo.RankStats.Wins, charInfo.RankStats.Draws, charInfo.RankStats.Loses));
    ranking->SetString(std::to_string(charInfo.RankStats.Rank));
    guild->SetString("");

    const auto albumButton = statusPanel->FindChild<Gx::Button>(Resource::MyRoom::Status::IDC_BUTTON_MY_ALBUM);
    albumButton->SetClickCallback([=] (auto&, auto&)
    {
        ShowDialog("Album mode is currently not available", DialogStyle::Information, false, [=] (auto)
        {
            m_mixer.Play(*sfxAccept, Sound::Channel::SFX);
        });
    });

    const auto sellButton = Instantiate<Gx::Button>(Resource::MyRoom::IDC_BUTTON_SELL);
    sellButton->SetClickCallback([=, &charInfo] (auto&, auto&)
    {
        if (statusPanel->IsVisible())
            return;

        if (!m_selectedItem)
        {
            ShowDialog("No selected item.", DialogStyle::Information);
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
            ShowDialog("Selected item cannot be sold.", DialogStyle::Information);
            return;
        }

        const sf::String message = fmt::format(L"Item: {}\nPrice: {} {}\n\nAre you sure about selling the item?",
            m_selectedItem->GetName(), price, sf::String(std::string(magic_enum::enum_name(currency))));

        ShowDialog(message, DialogStyle::OkCancel, false, [=, &charInfo] (auto accepted)
        {
            if (!accepted)
            {
                m_mixer.Play(*sfxCancel, Sound::Channel::SFX);
                return;
            }

            std::size_t slotID = charInfo.Inventory.size() + 1;
            for (std::size_t i = 0; i < charInfo.Inventory.size(); i++)
            {
                if (charInfo.Inventory[i] == m_selectedItem->GetID())
                {
                    slotID = i;
                    break;
                }
            }

            if (slotID >= charInfo.Inventory.size())
                return;

            m_shopService.SellItem(slotID, [=, &charInfo] (const SellItemResponse& response)
            {
                Invoke([=, &charInfo]
                {
                    if (response.Result == SellItemResult::Failed)
                    {
                        ShowDialog("Selected item cannot be sold.", DialogStyle::Information);
                        return;
                    }

                    charInfo.Inventory[response.SlotID] = 0;
                    m_inventory[response.SlotID] = Item{};

                    charInfo.Wallet = CharacterInfo::WalletInfo
                    {
                        response.Gem,
                        response.Cash
                    };

                    m_selectedItem = nullptr;
                    m_mixer.Play(*sfxAccept, Sound::Channel::SFX);

                    m_session.Save();
                    Invalidate();
                });
            });
        });
    });

    const auto inventoryButton = Instantiate<Gx::ToggleButton>(Resource::MyRoom::IDC_TOGGLE_INVENTORY);
    inventoryButton->SetCheckStateChangeCallback([=] (auto& sender)
    {
        statusPanel->SetVisible(sender.IsChecked());
        statusPanel->SetEnabled(sender.IsChecked());
        bagList->SetEnabled(!sender.IsChecked());
        equipmentsContainer->SetVisible(!sender.IsChecked());
        equipmentsContainer->SetEnabled(!sender.IsChecked());
    });

    const auto shopButton = Instantiate<Gx::Button>(Resource::MyRoom::IDC_BUTTON_SHOP);
    shopButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateItemShop>();
    });

    const auto backButton = Instantiate<Gx::Button>(Resource::MyRoom::IDC_BUTTON_BACK);
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        if (const auto sfx = Find<sf::Sound>(Sound::Effects::EF_35))
            m_mixer.Play(*sfx);

        GetDirector().Dismiss<StateRoom>();
    });

    bgm->setLooping(true);
    m_mixer.Play(*bgm, Sound::Channel::BGM);

    Invalidate();
}

void StateMyRoom::Invalidate()
{
    const auto charInfo  = &m_session.GetCharacterInfo();
    const auto avatar    = Instantiate<Avatar>(Resource::MyRoom::IDC_AVATAR);
    const auto container = Instantiate<Gx::UiContainer>(Resource::MyRoom::IDC_CONTAINER_EQUIPMENTS);
    const auto sfxClick  = Instantiate<sf::Sound>(Sound::Effects::EF_25);

    const auto bagList  = Instantiate<Gx::List>(Resource::MyRoom::IDC_LIST_BAG);
    const auto bagSlots = bagList->GetChildren();

    Gx::UiContainer* currentSlot = nullptr;
    auto inventory = std::vector<Item*>();
    for (auto& item : m_inventory)
    {
        if (charInfo->EquippedItemIDs.find(item.GetID()) == charInfo->EquippedItemIDs.end())
            inventory.push_back(&item);
    }

    // TODO: Detect vertical count?
    constexpr unsigned int verticalCount = 2; //bagList->GetVerticalCount();
    for (std::size_t i = 0, j = m_bagCurrentPage * verticalCount; i < bagSlots.size(); i++)
    {
        const auto slot = dynamic_cast<Gx::UiContainer*>(bagSlots[i]);
        if (!slot)
            continue;

        slot->SetVisible(false);
        slot->SetClickCallback(nullptr);
        slot->SetDoubleClickCallback(nullptr);

        if (j >= inventory.size())
            continue;

        const auto target = j;
        const auto item = inventory[j++];
        unsigned int quantity = 0;

        if (const auto it = std::find(charInfo->Inventory.begin(), charInfo->Inventory.end(), item->GetID()); it != charInfo->Inventory.end())
            quantity = it->Quantity;

        currentSlot = item == m_selectedItem ? slot : currentSlot;
        const auto thumbnail = slot->FindChild<Gx::Image>(Resource::MyRoom::Item::IDC_IMAGE_THUMBNAIL);
        if (item->GetID() == 0)
            thumbnail->SetTexCoords({});
        else if (item->GetSmallThumbnail().GetTexture())
            thumbnail->SetTexture(*item->GetSmallThumbnail().GetTexture(), true);
        else if (item->GetLargeThumbnail().GetTexture())
            thumbnail->SetTexture(*item->GetLargeThumbnail().GetTexture(), true);

        if (const auto quantityLabel = slot->FindChild<Gx::Label>(Resource::MyRoom::Item::IDC_TEXT_QUANTITY))
        {
            if (quantity > 0)
                quantityLabel->SetString(std::to_string(quantity));
            else
                quantityLabel->SetString(std::string());
        }

        slot->SetVisible(true);
        slot->SetClickCallback([=] (auto&, auto&)
        {
            m_mixer.Play(*sfxClick, Sound::Channel::SFX);
            if (m_selectedItem == item || item->GetID() == 0)
                return;

            m_selectedItem = item;
            m_bagSelectIndicator->SetVisible(true);

            slot->AddChild(*m_bagSelectIndicator);
        });

        slot->SetDoubleClickCallback([=] (auto&, auto&)
        {
            if (!item || item->GetID() == 0)
                return;

            if (item->GetType() == EquipmentType::AttributiveItem || quantity > 1)
            {
                if (const auto dialog = Instantiate<Gx::Dialog>(Resource::MyRoom::IDC_DIALOG_SKILL_INFO); dialog)
                {
                    const auto nameLabel        = dialog->FindChild<Gx::Label>(Resource::MyRoom::SkillInfo::IDC_TEXT_ITEM_NAME);
                    const auto quantityLabel    = dialog->FindChild<Gx::Label>(Resource::MyRoom::SkillInfo::IDC_TEXT_ITEM_QUANTITY);
                    const auto skillLabel       = dialog->FindChild<Gx::Label>(Resource::MyRoom::SkillInfo::IDC_TEXT_ITEM_SKILL);
                    const auto descriptionLabel = dialog->FindChild<Gx::Label>(Resource::MyRoom::SkillInfo::IDC_TEXT_ITEM_DESCRIPTION);
                    const auto skillThumbnail   = dialog->FindChild<Gx::Image>(Resource::MyRoom::SkillInfo::IDC_IMAGE_ITEM_THUMBNAIL);

                    nameLabel->SetString(item->GetName());
                    quantityLabel->SetString(quantity > 0 ? std::to_string(quantity) : "-"); // L"\u221E"
                    skillLabel->SetString(item->GetName().substring(0, item->GetName().find(' ')));
                    descriptionLabel->SetString(item->GetDescription());

                    constexpr unsigned int bounds = 160;
                    auto string = item->GetDescription();

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

                        const auto position = descriptionLabel->FindCharacterPosition(c);
                        if (position.x > descriptionLabel->GetPosition().x + bounds)
                        {
                            string.replace(checkpoint, 1, "\n");
                            descriptionLabel->SetString(string);

                            c = 0;
                        }
                    }

                    if (const auto texture = item->GetLargeThumbnail().GetTexture())
                        skillThumbnail->SetTexture(*texture, true);

                    Present(*dialog, Gx::PresentationContext::Default);
                }

                return;
            }

            if (m_busy)
                return;

            m_busy = true;
            m_service.Equip
            (
                EquipItemRequest{ GetItemEquipSlotType(item->GetType()), target },
                [=] (const EquipItemResponse& response)
                {
                    m_busy = false;
                    if (response.Invalid)
                        return;

                    Invoke([=]
                    {
                        avatar->Equip(m_inventory[response.SlotID]);

                        charInfo->EquippedItemIDs.insert(response.NewEquippedItemId);
                        charInfo->EquippedItemIDs.erase(response.PreviousEquippedItemId);
                        charInfo->Inventory[response.SlotID] = CharacterInfo::ItemInfo{response.PreviousEquippedItemId};
                        m_inventory[response.SlotID] = m_items.Create(response.PreviousEquippedItemId);

                        m_selectedItem = nullptr;
                        m_session.Save();

                        Invalidate();
                    });
                },
                [=] (const auto& ex)
                {
                    Invoke([=]
                    {
                        ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [=] (bool)
                        {
                            GetDirector().Dismiss<StatePlanet>();
                        });
                    });
                }
            );
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
    const auto instrument = Instantiate<Gx::Image>(Resource::MyRoom::IDC_IMAGE_INSTRUMENT);
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
        InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_HAIR), EquipmentType::Costume, RenderPart::SmallThumbnail);
    else
        InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_HAIR), EquipmentType::Hair);

    InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_HAIR_ACCESSORIES),       EquipmentType::HairAccessories);
    InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_WINGS),                  EquipmentType::Wings);
    InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_ACCESSORIES),            EquipmentType::Accessories);
    InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_INSTRUMENT_ACCESSORIES), EquipmentType::InstrumentAccessories);
    InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_PET),                    EquipmentType::Pet);
    InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_TOP),                    EquipmentType::Top);
    InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_PANTS),                  EquipmentType::Pants);
    InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_SHOES),                  EquipmentType::Shoes);
    InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_GLASSES),                EquipmentType::Glasses);
    InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_EARRINGS),               EquipmentType::Earrings);
    InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_NECKLACE),               EquipmentType::Necklace);
    InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_GLOVES),                 EquipmentType::Gloves);
    InvalidateSlot(container->FindChild<Gx::Image>(Resource::MyRoom::IDC_IMAGE_CLOTHES_ACCESSORIES),    EquipmentType::ClothesAccessories);

    const auto scrollControls = Instantiate<Gx::UiContainer>(Resource::MyRoom::IDC_CONTAINER_BAG_SCROLL_CONTROLS);

    const auto bagScrollBar = scrollControls->FindChild<Gx::ScrollBar>(Resource::MyRoom::IDC_SCROLL_MYBAG);
    bagScrollBar->SetMaximumValue(inventory.size() < bagSlots.size() ? 0 : static_cast<int>(std::ceil(static_cast<float>(inventory.size() - bagSlots.size()) / verticalCount)));

    const auto currentGem = Instantiate<Gx::BitmapNumber>(Resource::MyRoom::IDC_NUMBER_GEM);
    currentGem->SetValue(charInfo->Wallet.Gem);

    const auto currentCash = Instantiate<Gx::BitmapNumber>(Resource::MyRoom::IDC_NUMBER_CASH);
    currentCash->SetValue(charInfo->Wallet.Cash);
}

void StateMyRoom::InvalidateSlot(Gx::Image* slot, const EquipmentType type, RenderPart thumbnailType)
{
    if (!slot)
        return;

    if (thumbnailType != RenderPart::SmallThumbnail)
        thumbnailType = RenderPart::LargeThumbnail;

    const auto charInfo = &m_session.GetCharacterInfo();

    const auto avatar        = Instantiate<Avatar>(Resource::MyRoom::IDC_AVATAR);
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
            if (m_busy)
                return;

            m_busy = true;

            auto& inventory = charInfo->Inventory;
            auto slotIt = std::find_if(inventory.begin(), inventory.end(), [id = item->GetID()] (const auto& i) {
                return i.ID == 0;
            });

            const size_t target = slotIt != inventory.end() ? static_cast<size_t>(std::distance(inventory.begin(), slotIt)) : inventory.size();
            m_service.Equip
            (
                EquipItemRequest{ GetItemEquipSlotType(item->GetType()), target },
                [=] (const EquipItemResponse& response)
                {
                    m_busy = false;
                    if (response.Invalid)
                        return;

                    Invoke([=]
                    {
                        const auto sfxDress = Instantiate<sf::Sound>(Sound::Effects::EF_27_dress);
                        m_mixer.Play(*sfxDress);

                        avatar->Unequip(item->GetType());

                        charInfo->EquippedItemIDs.erase(response.PreviousEquippedItemId);
                        charInfo->Inventory[response.SlotID] = CharacterInfo::ItemInfo{response.PreviousEquippedItemId};
                        m_inventory[response.SlotID] = m_items.Create(response.PreviousEquippedItemId);

                        m_session.Save();
                        Invalidate();
                    });
                },
                [=] (const auto& ex)
                {
                    Invoke([=]
                    {
                        ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [=] (bool)
                        {
                            GetDirector().Dismiss<StatePlanet>();
                        });
                    });
                }
            );
        });
    }
}
