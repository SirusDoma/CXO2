#include <O2/Scenes/StatePlanet.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Tasks.hpp>
#include <Genode/Fx/Fade.hpp>
#include <Genode/UI.hpp>

#include <iostream>

void StatePlanet::Initialize()
{
    auto background = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("Metadata\\State_Planet\\Background.json");
    AddChild(background);

    auto tower = Gx::ResourceManager::Instance()->Create<Gx::Animation>("Metadata\\State_Planet\\Tower.json");
    AddChild(tower);

    auto button = Gx::ResourceManager::Instance()->Create<Gx::Button>("Metadata\\State_Planet\\Btn_Exit.json");
    button->SetClickCallback([] {
        std::cout << "Hello World" << std::endl;
    });
    AddChild(button);

    // Overlay fade in animation
    auto overlay = new Gx::Rectangle(sf::Vector2f(800, 600));
    overlay->SetFillColor(sf::Color::Black);
    AddChild(overlay);

    auto bgm = Gx::ResourceManager::Instance()->Create<sf::Music>("Metadata\\State_Planet\\Music.json");
    if (bgm)
        bgm->play();

    Run(new Gx::Sequence([=] { RemoveChild(overlay); }, {
        new Gx::Fade(overlay, 0, sf::seconds(4.5f))
    }));
}