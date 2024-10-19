#include <OTwo/States/StatePayment.hpp>
#include <OTwo/States/StateItemShop.hpp>
#include <OTwo/States/StateMusicShop.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/CartContext.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/BitmapNumber.hpp>

#include <SFML/Audio/Music.hpp>

StatePayment::StatePayment(Gx::Mixer& mixer, SessionContext& session, CartContext& cart) :
    m_mixer(mixer),
    m_session(session),
    m_cart(cart)
{
}

void StatePayment::Initialize()
{
    State::Initialize();

    const auto& player = m_session.GetCurrentPlayer();

    const auto currentGem = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_GEM");
    currentGem->SetValue(player.Gem);

    const auto currentCash = Instantiate<Gx::BitmapNumber>("IDC_NUMBER_CASH");
    currentCash->SetValue(player.Cash);

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        if (m_cart.GetCheckoutType() == CartContext::CheckoutType::Music)
            GetDirector().Present<StateMusicShop>();
        else
            GetDirector().Present<StateItemShop>();
    });

    const auto bgm = Instantiate<sf::Music>("BGM/bgLogin.ogg");
    bgm->setLooping(true);
    m_mixer.Play(bgm, "BGM");
}
