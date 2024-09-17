#include <OTwo/States/StateItemShop.hpp>
#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StateMyRoom.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/ScrollBar.hpp>
#include <Genode/UI/BitmapNumber.hpp>

#include <magic_enum.hpp>
#include <Genode/UI/Label.hpp>

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

    const auto& player      = m_session.GetCurrentPlayer();
    const auto bgm          = Instantiate<sf::Music>("BGM/bgItemShop.ogg");
    const auto sfxAccept    = Instantiate<sf::Sound>("bgEffect/02");
    const auto sfxCancel    = Instantiate<sf::Sound>("bgEffect/03");
    const auto sfxMyBagPrev = Instantiate<sf::Sound>("bgEffect/19_1");
    const auto sfxMyBagNext = Instantiate<sf::Sound>("bgEffect/19_2");

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

    const auto myBagContainer = Instantiate<Gx::UiContainer>("IDC_CONTAINER_MYBAG");
    m_myBagSelectIndicator = myBagContainer->FindChild<Gx::Image>("IDC_IMAGE_MYBAG_SELECT");
    m_myBagSelectIndicator->SetVisible(false);

    const auto bagList = myBagContainer->FindChild<Gx::List>("IDC_LIST_BAG");
    const auto bagSlots = bagList->GetChildren();

    m_myBagCurrentPage = 0;

    const auto bagScrollBar = myBagContainer->FindChild<Gx::ScrollBar>("IDC_SCROLL_MYBAG");
    bagScrollBar->SetMaximumValue(std::ceil(static_cast<float>(m_inventory.size()) / 2.f));
    bagScrollBar->SetValueChangedCallback([this, sfxMyBagPrev, sfxMyBagNext] (auto&, const float value)
    {
        if (value < m_myBagCurrentPage)
            m_mixer.Play(sfxMyBagPrev, "SFX");
        else
            m_mixer.Play(sfxMyBagNext, "SFX");

        m_myBagCurrentPage = static_cast<unsigned int>(value);
        InvalidateMyBag();
    });

    const auto bagScrollLeft = myBagContainer->FindChild<Gx::Button>("IDC_BUTTON_MYBAG_SCROLL_LEFT");
    bagScrollLeft->SetClickCallback([=] (auto&, auto&) { bagScrollBar->Decrease(); });

    const auto bagScrollRight = myBagContainer->FindChild<Gx::Button>("IDC_BUTTON_MYBAG_SCROLL_RIGHT");
    bagScrollRight->SetClickCallback([=] (auto&, auto&) { bagScrollBar->Increase(); });

    bagList->SetScrollWheelCallback([=] (auto&, auto& ev) {
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
        myBagContainer->SetVisible(true);
        myBagContainer->SetEnabled(true);

        cartContainer->SetVisible(false);
        cartContainer->SetEnabled(false);
    });

    cartButton->SetClickCallback([=] (auto&, auto&)
    {
        myBagContainer->SetVisible(false);
        myBagContainer->SetEnabled(false);

        cartContainer->SetVisible(true);
        cartContainer->SetEnabled(true);
    });

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateRoom>();
    });


    myBagButton->PerformClick();
    InvalidateMyBag();

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
        if (item->GetType() == EquipmentType::AttributiveItem && item->GetLargePreview() && item->GetLargePreview()->GetTexture())
            slot->SetTexture(*item->GetLargePreview()->GetTexture(), true);
        else if (item->GetSmallPreview() && item->GetSmallPreview()->GetTexture())
            slot->SetTexture(*item->GetSmallPreview()->GetTexture(), true);

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
