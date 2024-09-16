#include <OTwo/States/StateAvi.hpp>

#include <SFML/Audio/Music.hpp>

#include <Genode/Fx/Fade.hpp>
#include <Genode/Tasks/Action.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <OTwo/States/StatePlanet.hpp>

StateAvi::StateAvi(Gx::Mixer& mixer) :
    m_mixer(mixer)
{
}

void StateAvi::Initialize()
{
    State::Initialize();

    auto& director = GetDirector();
    const auto bgm = Instantiate<sf::Music>("STATE_AVI/IDC_MUSIC");
    m_mixer.Play(bgm, "BGM");

    auto& overlay = Create<Gx::Rectangle>(GetView().getSize());
    overlay.SetColor(sf::Color(0, 0, 0, 255));
    AddChild(overlay);

    auto& splash = Create<Gx::Sequence>([&]
        {
            director.Present<StatePlanet>();
        },
        Create<Gx::Fade>(overlay, 000, sf::seconds(2.5f)),
        Create<Gx::Fade>(overlay, 255, sf::seconds(2.5f)),
        Create<Gx::Action>([&] { AddChild(overlay); })
    );

    Run(splash);
}

bool StateAvi::Close(bool quit)
{
    return true;
}
