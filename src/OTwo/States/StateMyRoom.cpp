#include <OTwo/States/StateMyRoom.hpp>
#include <OTwo/States/StateRoom.hpp>

#include <OTwo/Avatar/Avatar.hpp>

#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/Contexts/SessionContext.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/List.hpp>
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
    const auto player = session.GetCurrentPlayer();

    const auto bgm     = Instantiate<sf::Music>("BGM/bgMyroom.ogg");
    const auto sfxPrev = Instantiate<sf::Sound>("bgEffect/19_1");
    const auto sfxNext = Instantiate<sf::Sound>("bgEffect/19_2");

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

    const auto statusPanel = Instantiate<Gx::Image>("IDC_IMAGE_STATUS");
    statusPanel->SetVisible(false);

    const auto inventoryButton = Instantiate<Gx::CheckBox>("IDC_CHECKBOX_INVENTORY");
    inventoryButton->SetCheckStateChangeCallback([=] (auto checkBox)
    {
        statusPanel->SetVisible(checkBox->IsChecked());
        equipmentsContainer->SetVisible(!checkBox->IsChecked());
    });

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([this, slots = bagSlots] (auto&, auto&)
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
    const auto instrument = container->FindChild<Gx::Image>("IDC_IMAGE_INSTRUMENT");
    instrument->SetVisible(false);
    instrument->SetDoubleClickCallback(nullptr);
    for (auto type : { EquipmentType::Keyboard, EquipmentType::Bass, EquipmentType::Drum, EquipmentType::Guitar })
    {
        if (auto it = equippedItems.find(type); it != equippedItems.end())
        {
            InvalidateSlot(container->FindChild<Gx::Image>("IDC_IMAGE_INSTRUMENT"), type);
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
