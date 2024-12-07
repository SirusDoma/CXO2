#include <OTwo/States/StatePayment.hpp>
#include <OTwo/States/StateItemShop.hpp>
#include <OTwo/States/StateMusicShop.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/Contexts/CartContext.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/BitmapNumber.hpp>

#include <SFML/Audio/Music.hpp>

StatePayment::StatePayment(Gx::AudioMixer& mixer, SessionContext& session, ItemFactory& items, CartContext& cart) :
    m_mixer(mixer),
    m_session(session),
    m_items(items),
    m_cart(cart)
{
}

void StatePayment::Initialize()
{
    State::Initialize();

    auto& player            = m_session.GetCurrentPlayer();
    const auto& itemData    = m_items.GetItemData();
    const auto& setInfoData = m_items.GetSetInfoData();

    for (const auto cartItem : m_cart.GetItems())
    {
        auto itemIDs = std::vector<unsigned int>();
        if (cartItem.Type == CartItemType::EquipmentSet)
        {
            const auto& sets = setInfoData.Require->Sets.value();
            if (auto set = sets.find(cartItem.ID); set != sets.end())
            {
                for (const auto itemID : set->second.Require->Items.value())
                    itemIDs.push_back(itemID);
            }
        }
        else
            itemIDs.push_back(cartItem.ID);

        for (auto itemID : itemIDs)
        {
            const auto it = std::find_if(player.Inventory.begin(), player.Inventory.end(), [itemID] (auto id)
            {
                return id == itemID;
            });

            if (it == player.Inventory.end())
                player.Inventory.push_back(itemID);
        }
    }
    m_session.Save();

    const auto currentGem = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_GEM");
    currentGem->SetValue(player.Gem);

    const auto currentCash = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_CASH");
    currentCash->SetValue(player.Cash);

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Dismiss();
    });

    const auto bgm = Instantiate<sf::Music>("BGM/bgLogin.ogg");
    bgm->setLooping(true);
    m_mixer.Play(*bgm, "BGM");
}
