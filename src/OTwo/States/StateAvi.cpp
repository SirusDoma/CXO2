#include <OTwo/States/StateAvi.hpp>
#include <OTwo/States/StatePlanet.hpp>

#include <OTwo/StringTable/Identifiers/Sound.hpp>

#include <Genode/Tween/Fade.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <SFML/Audio/Music.hpp>

using namespace StringTable::Identifiers;

StateAvi::StateAvi(Gx::AudioMixer& mixer) :
    m_mixer(mixer)
{
}

void StateAvi::Initialize()
{
    State::Initialize();

    auto& director = GetDirector();
    const auto bgm = Instantiate<sf::Music>(Sound::BGM::BG_LOGO);
    m_mixer.Play(*bgm, Sound::Channel::BGM);

    auto& overlay = Create<Gx::Rectangle>(GetView().getSize());
    overlay.SetColor(sf::Color(0, 0, 0, 255));
    AddChild(overlay);

    Run<Gx::Sequence>([&]
        {
            director.Present<StatePlanet>();
        },
        Gx::Fade(overlay, 000, sf::seconds(2.5f)),
        Gx::Fade(overlay, 255, sf::seconds(2.5f))
    );
}

bool StateAvi::OnAppClose()
{
    return true;
}
