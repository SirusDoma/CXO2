#include <OTwo/States/StatePayment.hpp>
#include <OTwo/States/StateItemShop.hpp>

#include <Genode/UI/Button.hpp>

#include <SFML/Audio/Music.hpp>

StatePayment::StatePayment(Gx::Mixer& mixer) :
    m_mixer(mixer)
{
}

void StatePayment::Initialize()
{
    State::Initialize();

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        // TODO: Use cart context to determine previous scene
        GetDirector().Present<StateItemShop>();
    });

    const auto bgm = Instantiate<sf::Music>("BGM/bgLogin.ogg");
    bgm->setLooping(true);
    m_mixer.Play(bgm, "BGM");
}
