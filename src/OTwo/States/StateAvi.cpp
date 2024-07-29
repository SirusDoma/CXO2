#include <OTwo/States/StateAvi.hpp>

#include <SFML/Audio/Music.hpp>

#include <Genode/Fx/Fade.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <OTwo/States/StatePlanet.hpp>
#include <OTwo/States/StateTest.hpp>

StateAvi::StateAvi(State &&state) :
    State(std::move(state))
{
}

void StateAvi::Initialize()
{
    State::Initialize();

    auto& director  = GetDirector();
    auto& mixer     = Require<Gx::Mixer>();
    auto background = Load<Gx::Sprite>("STATE_AVI/IDC_IMAGE_STATE_AVI");
    auto bgm        = Load<sf::Music>("STATE_AVI/IDC_MUSIC");
    mixer.Play(bgm, "BGM");

    auto splash = Create<Gx::Sequence>([&]
        {
            director.Present<StatePlanet>();
        },
        Gx::Sequence::ListOf(
        {
            Create<Gx::Fade>(background, 255, sf::seconds(2.25f)),
            Create<Gx::Fade>(background, 000, sf::seconds(2.25f))
        })
    );

    Run(splash);
}

bool StateAvi::Close(bool quit)
{
    return true;
}
