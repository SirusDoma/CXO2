#include <O2/States/StateAvi.hpp>

#include <SFML/Audio/Music.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Fx/Fade.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <O2/States/StatePlanet.hpp>

void StateAvi::Initialize()
{
    auto sprite = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("Metadata\\State\\Avi\\Background.json");
    AddChild(sprite);

    auto bgm = Gx::ResourceManager::Instance()->Create<sf::Music>("Metadata\\State\\Avi\\Music.json");
    if (bgm)
        bgm->play();

    auto sequence = new Gx::Sequence([=] { GetDirector()->SetScene(new StatePlanet()); },
        {
            new Gx::Fade(sprite, 255, sf::seconds(2.25f)),
            new Gx::Fade(sprite, 000, sf::seconds(2.25f))
        }
    );

    Run(sequence);
}

bool StateAvi::Close(bool quit)
{
    return false;
}
