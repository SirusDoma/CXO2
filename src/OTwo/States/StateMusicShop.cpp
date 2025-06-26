#include <OTwo/States/StateMusicShop.hpp>
#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StatePayment.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/CartContext.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>

#include <OTwo/StringTable/Identifiers/Sound.hpp>
#include <OTwo/StringTable/Identifiers/MusicShop.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/Gauge.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/ToggleButton.hpp>

#include <SFML/Audio/Music.hpp>

#include <fmt/format.h>

using namespace StringTable::Identifiers;

StateMusicShop::StateMusicShop(Gx::AudioMixer& mixer, SessionContext& session, CartContext& cart, ItemFactory& items) :
    m_mixer(mixer),
    m_session(session),
    m_cart(cart),
    m_items(items),
    m_cartCurrentPage(0),
    m_selector(nullptr)
{
}

void StateMusicShop::Initialize()
{
    State::Initialize();

    const auto& player = m_session.GetCurrentPlayer();
    const auto bgm     = Instantiate<sf::Music>(Sound::BGM::BG_MUSIC_SHOP);

    const auto currentGem = Instantiate<Gx::BitmapNumber>(Resource::MusicShop::IDC_NUMBER_GEM);
    currentGem->SetValue(player.Gem);

    const auto currentCash = Instantiate<Gx::BitmapNumber>(Resource::MusicShop::IDC_NUMBER_CASH);
    currentCash->SetValue(player.Cash);

    const auto shopContainer     = Instantiate<Gx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_SHOP);
    const auto musicContainer    = Instantiate<Gx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_MUSIC);
    const auto downloadContainer = Instantiate<Gx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_DOWNLOAD);
    const auto cartContainer     = Instantiate<Gx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_CART);
    const auto downloadTabButton = Instantiate<Gx::Button>(Resource::MusicShop::IDC_BUTTON_DOWNLOAD_TAB);
    const auto cartTabButton     = Instantiate<Gx::Button>(Resource::MusicShop::IDC_BUTTON_CART_TAB);

    const auto showAllButton     = shopContainer->FindChild<Gx::Button>(Resource::MusicShop::IDC_BUTTON_SHOW_ALL);
    const auto showBuyableButton = shopContainer->FindChild<Gx::Button>(Resource::MusicShop::IDC_BUTTON_SHOW_BUYABLE);

    showAllButton->SetClickCallback([=] (auto&, auto&)
    {
        showAllButton->SetEnabled(false);
        showAllButton->SetVisible(false);

        showBuyableButton->SetEnabled(true);
        showBuyableButton->SetVisible(true);
    });

    showBuyableButton->SetClickCallback([=] (auto&, auto&)
    {
        showAllButton->SetEnabled(true);
        showAllButton->SetVisible(true);

        showBuyableButton->SetEnabled(false);
        showBuyableButton->SetVisible(false);
    });


    downloadTabButton->SetClickCallback([=] (auto&, auto&)
    {
        downloadContainer->SetEnabled(true);
        downloadContainer->SetVisible(true);

        cartContainer->SetEnabled(false);
        cartContainer->SetVisible(false);
    });

    cartTabButton->SetClickCallback([=] (auto&, auto&)
    {
        downloadContainer->SetEnabled(false);
        downloadContainer->SetVisible(false);

        cartContainer->SetEnabled(true);
        cartContainer->SetVisible(true);
    });

    const auto musicList = musicContainer->FindChild<Gx::List>(Resource::MusicShop::IDC_LIST_MUSIC);
    for (const auto child : musicList->GetChildren())
    {
        const auto item = dynamic_cast<Gx::UiContainer*>(child);
        if (!item)
            continue;

        if (const auto selector = item->FindChild<Gx::Image>(Resource::MusicShop::MusicItem::IDC_IMAGE_SELECTOR))
        {
            selector->SetVisible(false);
            item->SetFocusChangedCallback([=] (auto& sender, auto&)
            {
                selector->SetVisible(sender.IsFocused());
                if (const auto toggleButton = item->FindChild<Gx::ToggleButton>(Resource::MusicShop::MusicItem::IDC_TOGGLE_SELECT); sender.IsFocused() && toggleButton)
                    toggleButton->SetFocus(sender.IsFocused());
            });

            if (const auto toggleButton = item->FindChild<Gx::ToggleButton>(Resource::MusicShop::MusicItem::IDC_TOGGLE_SELECT))
            {
                toggleButton->SetClickCallback([=] (auto&, auto& ev)
                {
                    ev.Handled = true; // Use item click callback
                });

                toggleButton->SetFocusChangedCallback([=] (auto&, auto& ev)
                {
                    ev.State = item->IsFocused() ? Gx::Control::State::Hover : Gx::Control::State::Normal;
                });

                item->SetClickCallback([=] (auto&, auto&)
                {
                    toggleButton->SetCheckedState(!toggleButton->IsChecked());
                });
            }
        }
    }

    const auto musicGauge = downloadContainer->FindChild<Gx::Gauge>(Resource::MusicShop::Download::IDC_TEXT_DOWNLOAD_MUSIC_GAUGE);
    const auto totalGauge = downloadContainer->FindChild<Gx::Gauge>(Resource::MusicShop::Download::IDC_TEXT_DOWNLOAD_TOTAL_GAUGE);

    musicGauge->SetValue(70);
    totalGauge->SetValue(35);

    const auto buyButton = cartContainer->FindChild<Gx::Button>(Resource::MusicShop::Cart::IDC_BUTTON_BUY);
    buyButton->SetClickCallback([this] (auto&, auto&) { OnBuyButtonClicked(); });

    const auto giftButton = cartContainer->FindChild<Gx::Button>(Resource::MusicShop::Cart::IDC_BUTTON_GIFT);
    giftButton->SetClickCallback([this] (auto&, auto&) { OnGiftButtonClicked(); });

    const auto cartList           = cartContainer->FindChild<Gx::List>(Resource::MusicShop::Cart::IDC_LIST_CART);
    const auto cartPrevPageButton = cartContainer->FindChild<Gx::Button>(Resource::MusicShop::Cart::IDC_BUTTON_LEFT);
    const auto cartNextPageButton = cartContainer->FindChild<Gx::Button>(Resource::MusicShop::Cart::IDC_BUTTON_RIGHT);

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

    const auto backButton = Instantiate<Gx::Button>(Resource::MusicShop::IDC_BUTTON_BACK);
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateRoom>();
    });

    showBuyableButton->PerformClick();
    downloadTabButton->PerformClick();
    InvalidateCart();

    bgm->setLooping(true);
    m_mixer.Play(*bgm, Sound::Channel::BGM);
}

void StateMusicShop::OnBuyButtonClicked()
{
    if (m_cart.GetItems().size() == 0)
    {
        ShowDialog("Shopping bag is empty", DialogStyle::Information);
        return;
    }

    ShowDialog("Would you like to move\nto the transaction window?", DialogStyle::YesNo, false, [=] (const bool answer)
    {
        if (answer)
        {
            m_cart.SetCheckoutType(CartContext::CheckoutType::Music);
            m_mixer.Play(*Instantiate<sf::Sound>(Sound::Effects::EF_02), Sound::Channel::SFX);
            GetDirector().Present<StatePayment>();
        }
        else
            m_mixer.Play(*Instantiate<sf::Sound>(Sound::Effects::EF_03), Sound::Channel::SFX);
    });
}

void StateMusicShop::OnGiftButtonClicked()
{
    if (m_cart.GetItems().size() == 0)
    {
        ShowDialog("Shopping bag is empty", DialogStyle::Information);
        return;
    }

    ShowDialog("Gift is currently not available", DialogStyle::Information);
}


void StateMusicShop::InvalidateCart()
{
    const auto container = Instantiate<Gx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_CART);
    const auto cartList  = container->FindChild<Gx::List>(Resource::MusicShop::Cart::IDC_LIST_CART);
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
        else if (item.Type == CartItemType::Equipment)
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
        const auto id           = slot->FindChild<Gx::Label>(Resource::MusicShop::Cart::Item::IDC_TEXT_NUMBER);
        const auto name         = slot->FindChild<Gx::Label>(Resource::MusicShop::Cart::Item::IDC_TEXT_NAME);
        const auto type         = slot->FindChild<Gx::Image>(Resource::MusicShop::Cart::Item::IDC_IMAGE_ITEM_TYPE);
        const auto price        = slot->FindChild<Gx::Label>(Resource::MusicShop::Cart::Item::IDC_TEXT_PRICE);
        const auto deleteButton = slot->FindChild<Gx::Button>(Resource::MusicShop::Cart::Item::IDC_BUTTON_DELETE);

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
        else if (item.Type == CartItemType::Equipment)
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

        deleteButton->SetClickCallback([this, index = j - 1] (auto&, auto&)
        {
            m_cart.Remove(index);
            InvalidateCart();
        });
    }

    const auto currentPage = container->FindChild<Gx::BitmapNumber>(Resource::MusicShop::Cart::IDC_NUMBER_CURRENT_PAGE);
    const auto totalPage   = container->FindChild<Gx::BitmapNumber>(Resource::MusicShop::Cart::IDC_NUMBER_MAX_PAGE);
    const auto totalGem    = container->FindChild<Gx::BitmapNumber>(Resource::MusicShop::Cart::IDC_NUMBER_TOTAL_GEM);
    const auto totalCash   = container->FindChild<Gx::BitmapNumber>(Resource::MusicShop::Cart::IDC_NUMBER_TOTAL_CASH);

    currentPage->SetValue(maxPage > 0 ? m_cartCurrentPage + 1 : 0);
    totalPage->SetValue(maxPage);
    totalGem->SetValue(gem);
    totalCash->SetValue(cash);
}
