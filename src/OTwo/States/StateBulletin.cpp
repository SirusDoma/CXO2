#include <OTwo/States/StateBulletin.hpp>
#include <OTwo/States/StateRoom.hpp>

#include <Genode/UI/Button.hpp>

#include <SFML/Audio/Music.hpp>

StateBulletin::StateBulletin(Gx::AudioMixer& mixer) :
    m_mixer(mixer)
{
}

void StateBulletin::Initialize()
{
    State::Initialize();

    const auto backButton = Instantiate<Gx::Button>("IDC_BUTTON_BACK");
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateRoom>();
    });

    const auto bgm = Instantiate<sf::Music>("BGM/bgLogin.ogg");
    bgm->setLooping(true);
    m_mixer.Play(*bgm, "BGM");
}
