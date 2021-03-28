#include <O2/States/StateAvi.hpp>

#include <SFML/Audio/Music.hpp>

#include <Genode/Fx/Fade.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <O2/States/StatePlanet.hpp>

void StateAvi::Initialize()
{
    auto sprite = Create<Gx::Sprite>("Metadata/State/Avi/Background.json");
    AddChild(sprite);

    auto bgm = Create<sf::Music>("Metadata/State/Avi/Music.json", Gx::ResourceScope::Shared);
    Mixer::Play(bgm);

    auto sequence = new Gx::Sequence([=] { QueueEvent([=] { GetDirector().SetScene(new StatePlanet()); }); },
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
