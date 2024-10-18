#include <OTwo/States/StateMusicShop.hpp>
#include <OTwo/States/StateRoom.hpp>

#include <Genode/UI/Button.hpp>

#include <SFML/Audio/Music.hpp>

StateMusicShop::StateMusicShop(Gx::Mixer& mixer) :
    m_mixer(mixer)
{
}

void StateMusicShop::Initialize()
{
    State::Initialize();

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateRoom>();
    });

    const auto bgm = Instantiate<sf::Music>("BGM/bgMusicShop.ogg");
    bgm->setLooping(true);
    m_mixer.Play(bgm, "BGM");
}
