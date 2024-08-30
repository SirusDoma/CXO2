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
    State::State(std::move(state))
{
}

void StateMyRoom::Initialize()
{
    auto& mixer       = Require<Gx::Mixer>();
    auto& session     = Require<SessionContext>();
    const auto& items = Require<ItemFactory>();
    const auto bgm    = Instantiate<sf::Music>("BGM/bgMyroom.ogg");
    const auto player = session.GetCurrentPlayer();

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

    const auto bagList = Instantiate<Gx::List>("IDC_LIST_BAG");
    const auto bagSlots = bagList->GetChildren();

    m_bagCurrentPage = 0;
    m_bagMaxPage     = std::ceil(static_cast<float>(bagSlots.size()) / static_cast<float>(m_inventory.size()));

    for (std::size_t i = 0; i < bagSlots.size(); i++)
    {
        const auto slot = dynamic_cast<Gx::Image*>(bagSlots[i]);
        if (!slot)
            continue;

        slot->SetDoubleClickCallback(nullptr);
    }

    const auto bagScrollBar = Instantiate<Gx::ScrollBar>("IDC_SCROLL_MY_BAG");
    bagScrollBar->SetMaximumValue(m_bagMaxPage - 1);
    bagScrollBar->SetValueChangedCallback([this] (auto&, const float value)
    {
        m_bagCurrentPage = static_cast<unsigned int>(value);
        InvalidateBagList();
    });

    const auto bagScrollLeft = Instantiate<Gx::Button>("IDC_BUTTON_SCROLL_LEFT");
    bagScrollLeft->SetClickCallback([=] (auto&, auto&) { bagScrollBar->Decrease(); });

    const auto bagScrollRight = Instantiate<Gx::Button>("IDC_BUTTON_SCROLL_RIGHT");
    bagScrollRight->SetClickCallback([=] (auto&, auto&) { bagScrollBar->Increase(); });

    const auto statusPanel = Instantiate<Gx::Image>("IDC_IMAGE_STATUS");
    statusPanel->SetVisible(false);

    const auto inventoryButton = Instantiate<Gx::CheckBox>("IDC_CHECKBOX_INVENTORY");
    inventoryButton->SetCheckStateChangeCallback([=] (auto checkBox)
    {
        statusPanel->SetVisible(checkBox->IsChecked());
    });

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([this, slots = bagSlots] (auto&, auto&)
    {
        GetDirector().Present<StateRoom>();
    });

    bgm->setLoop(true);
    mixer.Play(bgm, "BGM");

    InvalidateBagList();
}

void StateMyRoom::InvalidateBagList()
{
    const auto avatar = Instantiate<Avatar>("IDC_AVATAR");
    auto& session     = Require<SessionContext>();
    auto player       = &session.GetCurrentPlayer();

    const auto bagList  = Instantiate<Gx::List>("IDC_LIST_BAG");
    const auto bagSlots = bagList->GetChildren();
    for (std::size_t i = 0, j = (m_bagCurrentPage * bagSlots.size()); i < bagSlots.size(); i++)
    {
        const auto slot = dynamic_cast<Gx::Image*>(bagSlots[i]);
        if (!slot)
            continue;

        slot->SetDoubleClickCallback(nullptr);
        if (j >= m_inventory.size())
        {
            slot->SetVisible(false);
            continue;
        }

        const auto item = m_inventory[j++];
        if (avatar->IsEquiped(item))
        {
            i--;
            continue;
        }

        if (item->GetType() == EquipmentType::AttributiveItem && item->GetLargePreview() && item->GetLargePreview()->GetTexture())
            slot->SetTexture(*item->GetLargePreview()->GetTexture(), true);
        else if (item->GetSmallPreview() && item->GetSmallPreview()->GetTexture())
            slot->SetTexture(*item->GetSmallPreview()->GetTexture(), true);

        slot->SetVisible(true);
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

            InvalidateBagList();
        });

    }
}
