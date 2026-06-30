#include <CXO2/States/StatePayment.hpp>
#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Avatar/ItemFactory.hpp>
#include <CXO2/Contexts/CartContext.hpp>

#include <CXO2/StringTable/Identifiers/Sound.hpp>
#include <CXO2/StringTable/Identifiers/Payment.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/BitmapNumber.hpp>

#include <SFML/Audio/Music.hpp>

namespace Cx
{
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

        auto& charInfo          = m_session.GetCharacterInfo();
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
                const auto it = std::find_if(charInfo.Inventory.begin(), charInfo.Inventory.end(), [itemID] (auto id)
                {
                    return id == itemID;
                });

                if (it == charInfo.Inventory.end())
                    charInfo.Inventory.push_back(itemID);
            }
        }
        m_cart.Clear();
        m_session.Save();

        const auto currentGem = Instantiate<Gx::BitmapNumber>(Resource::Payment::IDC_NUMBER_GEM);
        currentGem->SetValue(charInfo.Wallet.Gem);

        const auto currentCash = Instantiate<Gx::BitmapNumber>(Resource::Payment::IDC_NUMBER_CASH);
        currentCash->SetValue(charInfo.Wallet.Cash);

        const auto backButton = Instantiate<Gx::Button>(Resource::Payment::IDC_BUTTON_BACK);
        backButton->SetClickCallback([this] (auto&, auto&)
        {
            GetDirector().Dismiss();
        });

        const auto bgm = Instantiate<sf::Music>(Sound::BGM::BG_LOGIN);
        bgm->setLooping(true);
        m_mixer.Play(*bgm, Sound::Channel::BGM);
    }
}
