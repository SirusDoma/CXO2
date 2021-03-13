#include <O2/Scenes/StateAvi.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Tasks/Action.hpp>
#include <Genode/Fx/Fade.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <O2/Scenes/StatePlanet.hpp>

void StateAvi::Initialize()
{
    auto sprite = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("Metadata\\StateAvi\\Background.json");
    AddChild(sprite);

    auto bgm = Gx::ResourceManager::Instance()->Create<sf::Music>("Metadata\\StateAvi\\Music.json");
    if (bgm)
        bgm->play();

    auto sequence = new Gx::Sequence({
        new Gx::Fade(sprite, 255, 2250),
        new Gx::Fade(sprite, 0, 2250),
        new Gx::Action([=] { GetDirector()->SetScene(new StatePlanet()); })
    });

    Run(sequence);
}