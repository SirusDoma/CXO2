#include <OTwo/States/StateAvi.hpp>
#include <OTwo/States/StatePlanet.hpp>

#include <Genode/Tween/Fade.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <SFML/Audio/Music.hpp>

StateAvi::StateAvi(Gx::AudioMixer& mixer) :
    m_mixer(mixer)
{
}

void StateAvi::Initialize()
{
    State::Initialize();

    auto& director = GetDirector();
    const auto bgm = Instantiate<sf::Music>("BGM/bgLogo.ogg");
    m_mixer.Play(*bgm, "BGM");

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

bool StateAvi::Close(bool quit)
{
    return true;
}
