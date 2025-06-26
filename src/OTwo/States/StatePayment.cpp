#include <OTwo/States/StatePayment.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/Contexts/CartContext.hpp>

#include <OTwo/StringTable/Identifiers/Sound.hpp>
#include <OTwo/StringTable/Identifiers/Payment.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/BitmapNumber.hpp>

#include <SFML/Audio/Music.hpp>

using namespace StringTable::Identifiers;

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
            const auto& sets = setInfoData.Sets;
            if (auto set = sets.find(cartItem.ID); set != sets.end())
            {
                for (const auto itemID : set->second.ItemsIDs)
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
    m_cart.Clear();
    m_session.Save();

    const auto currentGem = Instantiate<Gx::BitmapNumber>(Resource::Payment::IDC_NUMBER_GEM);
    currentGem->SetValue(player.Gem);

    const auto currentCash = Instantiate<Gx::BitmapNumber>(Resource::Payment::IDC_NUMBER_CASH);
    currentCash->SetValue(player.Cash);

    const auto backButton = Instantiate<Gx::Button>(Resource::Payment::IDC_BUTTON_BACK);
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Dismiss();
    });

    const auto bgm = Instantiate<sf::Music>(Sound::BGM::BG_LOGIN);
    bgm->setLooping(true);
    m_mixer.Play(*bgm, Sound::Channel::BGM);
}
