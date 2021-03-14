#include <O2/Scenes/StatePlanet.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Tasks.hpp>
#include <Genode/Fx/Fade.hpp>
#include <Genode/UI.hpp>

void StatePlanet::Initialize()
{
    auto bgm = Gx::ResourceManager::Instance()->Create<sf::Music>("Metadata\\State_Planet\\Music.json");
    if (bgm)
        bgm->play();

    auto background = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("Metadata\\State_Planet\\Background.json");
    AddChild(background);

    auto tower = Gx::ResourceManager::Instance()->Create<Gx::Animation>("Metadata\\State_Planet\\Tower.json");
    AddChild(tower);

    auto button = Gx::ResourceManager::Instance()->Create<Gx::Button>("Metadata\\State_Planet\\Btn_Exit.json");
    AddChild(button);

    // Overlay fade in animation
    auto overlay = new Gx::Rectangle(sf::Vector2f(800, 600));
    overlay->SetFillColor(sf::Color::Black);
    AddChild(overlay);

    Run(new Gx::Sequence({
        new Gx::Fade(overlay, 0, 4500),
        new Gx::Action([=] { RemoveChild(overlay); })
    }));
}