#include <O2/States/StatePlanet.hpp>

#include <Genode/System/Application.hpp>
#include <Genode/IO/ResourceManager.hpp>

#include <Genode/Tasks.hpp>
#include <Genode/Fx.hpp>
#include <Genode/UI.hpp>

#include <O2/States/StateRoom.hpp>

void StatePlanet::Initialize()
{
    State::Initialize();

    auto background = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("Metadata/State/Planet/Background.json");
    AddChild(background);

    auto tower = Gx::ResourceManager::Instance()->Create<Gx::Animation>("Metadata/State/Planet/Tower.json");
    AddChild(tower);

    auto exitButton = Gx::ResourceManager::Instance()->Create<Gx::Button>("Metadata/State/Planet/Btn_Exit.json");
    exitButton->SetClickCallback([=] (auto _) { Gx::Application::Instance()->Close(); });
    AddChild(exitButton);

    auto philix   = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Philix.json");
    auto kleo     = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Kleo.json");
    auto kaliope  = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Kaliope.json");
    auto euta     = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Euta.json");
    auto thalo    = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Thalo.json");
    auto melpomin = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Melpomin.json");

    m_container = new Gx::UiContainer();
    m_container->AddChild(philix, kleo, kaliope, euta, thalo, melpomin);
    std::unordered_map<Planet, Gx::RadioButton*> planets = {
        {Planet::Melpomin, melpomin},
        {Planet::Thalo,    thalo},
        {Planet::Euta,     euta},
        {Planet::Kaliope,  kaliope},
        {Planet::Kleo,     kleo},
        {Planet::Philix,   philix}
    };

    static auto clickSfx = Gx::ResourceManager::Instance()->Create<sf::Sound>("Metadata/State/Planet/Sound/Click.json");
    for (auto [planet, radio] : planets)
    {
        radio->SetCheckStateChangeCallback([this, p = planet] (auto sender) {
            if (!sender->IsChecked())
                return;

            Play(clickSfx, "sfx");
            m_container->SetEnabled(false);
            m_channelBoard->Show(p, [=] { OnEnterPlanet(p); });
        });
    }

    AddChild(m_container);

    m_channelBoard = new ChannelBoard(this);
    m_channelBoard->SetEnterChannelCallback([=] (auto channel) { OnEnterChannel(channel); });
    AddChild(m_channelBoard);

    // Overlay fade in animation
    auto overlay = new Gx::Rectangle(sf::Vector2f(800, 600));
    overlay->SetFillColor(sf::Color::White);
    AddChild(overlay);

    m_bgm = Gx::ResourceManager::Instance()->Create<sf::Music>("Metadata/State/Planet/Music.json");
    Play(m_bgm, "BGM");

    Run(new Gx::Sequence([=] { RemoveChild(overlay); }, {
        new Gx::Fade(overlay, 0, sf::seconds(2.5f))
    }));
}

void StatePlanet::OnEnterPlanet(Planet planet)
{
    auto planetInfo = PlanetInfo();
    planetInfo.Planet = planet;

    for (int x = 0; x < 2; x++)
    {
        for (int i = 5; i < 25; i++)
        {
            auto channel = ChannelInfo();
            channel.Population = static_cast<int>((i / 20.f) * 100.f);

            planetInfo.Channels.push_back(channel);
        }
    }

    m_channelBoard->UpdateChannelList(planetInfo);
    m_container->SetEnabled(true);
}

void StatePlanet::OnEnterChannel(ChannelInfo channel)
{
    QueueEvent([=] { GetDirector()->SetScene(new StateRoom()); } );
}
