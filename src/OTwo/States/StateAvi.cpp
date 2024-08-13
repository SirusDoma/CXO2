#include <OTwo/States/StateAvi.hpp>

#include <SFML/Audio/Music.hpp>

#include <Genode/Fx/Fade.hpp>
#include <Genode/Tasks/Action.hpp>
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
    auto background = Instantiate<Gx::Sprite>("STATE_AVI/IDC_IMAGE_STATE_AVI");
    auto bgm        = Instantiate<sf::Music>("STATE_AVI/IDC_MUSIC");
    mixer.Play(bgm, "BGM");

    auto overlay = Create<Gx::Rectangle>(GetView().getSize());
    overlay->SetColor(sf::Color(0, 0, 0, 255));
    AddChild(overlay);

    const auto splash = Create<Gx::Sequence>([&]
        {
            director.Present<StatePlanet>();
        },
        Gx::Sequence::ListOf(
        {
            Create<Gx::Fade>(overlay, 000, sf::seconds(2.5f)),
            Create<Gx::Fade>(overlay, 255, sf::seconds(2.5f)),
            Create<Gx::Action>([=] { AddChild(overlay); }),

        })
    );

    Run(splash);
}

bool StateAvi::Close(bool quit)
{
    return true;
}
