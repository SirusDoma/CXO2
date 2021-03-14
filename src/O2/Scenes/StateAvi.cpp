#include <O2/Scenes/StateAvi.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Fx/Fade.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <O2/Scenes/StatePlanet.hpp>

void StateAvi::Initialize()
{
    auto sprite = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("Metadata\\State_Avi\\Background.json");
    AddChild(sprite);

    auto bgm = Gx::ResourceManager::Instance()->Create<sf::Music>("Metadata\\State_Avi\\Music.json");
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