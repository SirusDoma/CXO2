#include <O2/States/StateAvi.hpp>

#include <SFML/Audio/Music.hpp>

#include <Genode/Fx/Fade.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <O2/States/StatePlanet.hpp>

void StateAvi::Initialize()
{
    auto& app   = GetApplication();
    auto& mixer = app.Require<Gx::Mixer>();

    auto sprite = Create<Gx::Sprite>("Interface/Metadata/State/Avi/Background.json");
    AddChild(sprite);

    auto bgm = mixer.Create<sf::Music>("Interface/Metadata/State/Avi/Music.json");
    mixer.Play(bgm, "BGM");

    auto sequence = new Gx::Sequence([=] { QueueSceneEvent([=] { GetDirector().SetScene(new StatePlanet()); }); },
        {
            new Gx::Fade(sprite, 255, sf::seconds(2.25f)),
            new Gx::Fade(sprite, 000, sf::seconds(2.25f))
        }
    );

    Run(sequence);
}

bool StateAvi::Close(bool quit)
{
    return true;
}
