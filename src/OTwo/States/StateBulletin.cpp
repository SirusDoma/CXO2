#include <OTwo/States/StateBulletin.hpp>
#include <OTwo/States/StateRoom.hpp>

#include <OTwo/StringTable/Identifiers/Sound.hpp>
#include <OTwo/StringTable/Identifiers/Bulletin.hpp>

#include <Genode/UI/Button.hpp>

#include <SFML/Audio/Music.hpp>

using namespace StringTable::Identifiers;

StateBulletin::StateBulletin(Gx::AudioMixer& mixer) :
    m_mixer(mixer)
{
}

void StateBulletin::Initialize()
{
    State::Initialize();

    const auto backButton = Instantiate<Gx::Button>(Resource::Bulletin::IDC_BUTTON_BACK);
    backButton->SetClickCallback([this] (auto&, auto&)
    {
        GetDirector().Present<StateRoom>();
    });

    const auto bgm = Instantiate<sf::Music>(Sound::BGM::BG_LOGIN);
    bgm->setLooping(true);
    m_mixer.Play(*bgm, Sound::Channel::BGM);
}
