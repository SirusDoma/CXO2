#include <O2/Scenes/StatePlanet.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Tasks.hpp>
#include <Genode/Fx/Fade.hpp>

void StatePlanet::Initialize()
{
    auto bgm = Gx::ResourceManager::Instance()->Create<sf::Music>("Metadata\\StatePlanet\\Music.json");
    if (bgm)
        bgm->play();

    auto background = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("Metadata\\StatePlanet\\Background.json");
    AddChild(background);

    auto tower = Gx::ResourceManager::Instance()->Create<Gx::Animation>("Metadata\\StatePlanet\\Tower.json");
    tower->SetPosition(211, 165);
    AddChild(tower);

    // Overlay fade in animation
    auto overlay = new Gx::Rectangle(sf::Vector2f(800, 600));
    overlay->SetFillColor(sf::Color::Black);
    AddChild(overlay);

    Run(new Gx::Sequence({
        new Gx::Fade(overlay, 0, 4500),
        new Gx::Action([=] { RemoveChild(overlay); })
    }));
}