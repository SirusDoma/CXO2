#include <CXO2/States/StateBulletin.hpp>
#include <CXO2/States/StateRoom.hpp>

#include <CXO2/Constants/Identifiers/Sound.hpp>
#include <CXO2/Constants/Identifiers/Bulletin.hpp>

#include <Genode/UI/Button.hpp>

#include <SFML/Audio/Music.hpp>

namespace Cx
{
    using namespace Constants::Identifiers;

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
}
