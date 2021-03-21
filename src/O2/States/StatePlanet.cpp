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

    auto background = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("Metadata\\State\\Planet\\Background.json");
    AddChild(background);

    auto tower = Gx::ResourceManager::Instance()->Create<Gx::Animation>("Metadata\\State\\Planet\\Tower.json");
    AddChild(tower);

    auto exitButton = Gx::ResourceManager::Instance()->Create<Gx::Button>("Metadata\\State\\Planet\\Btn_Exit.json");
    exitButton->SetClickCallback([=] { Gx::Application::Instance()->Close(); });
    AddChild(exitButton);

    auto philix   = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata\\State\\Planet\\Btn_Philix.json");
    auto kleo     = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata\\State\\Planet\\Btn_Kleo.json");
    auto kaliope  = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata\\State\\Planet\\Btn_Kaliope.json");
    auto euta     = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata\\State\\Planet\\Btn_Euta.json");
    auto thalo    = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata\\State\\Planet\\Btn_Thalo.json");
    auto melpomin = Gx::ResourceManager::Instance()->Create<Gx::RadioButton>("Metadata\\State\\Planet\\Btn_Melpomin.json");

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

    static auto sfx = sf::Sound();
    static auto sbf = sf::SoundBuffer();

    Gx::Uint8 *data;
    auto size = Gx::ResourceManager::Instance()->GetResourceData("Planet/click1", &data);
    if (sbf.loadFromMemory(data, size))
        sfx.setBuffer(sbf);

    for (auto [planet, radio] : planets)
    {
        radio->SetCheckStateChangeCallback([this, p = planet] {
            if (m_channelBoard->InTransition())
                return;

            sfx.play();
            ShowChannelBoard(p);
        });
    }

    AddChild(m_container);

    m_channelBoard = new ChannelBoard();
    m_channelBoard->SetEnterChannelCallback([=] (auto channel) { OnEnterChannel(channel); });
    AddChild(m_channelBoard);

    // Overlay fade in animation
    auto overlay = new Gx::Rectangle(sf::Vector2f(800, 600));
    overlay->SetFillColor(sf::Color::White);
    AddChild(overlay);

    m_bgm = Gx::ResourceManager::Instance()->Create<sf::Music>("Metadata\\State\\Planet\\Music.json");
    m_bgm->play();

    Run(new Gx::Sequence([=] { RemoveChild(overlay); }, {
        new Gx::Fade(overlay, 0, sf::seconds(2.5f))
    }));
}

void StatePlanet::ShowChannelBoard(Planet planet)
{
    m_channelBoard->Show(planet, [=] {
        m_container->SetEnabled(true);
        GetChannelCount(planet);
    });
}

void StatePlanet::GetChannelCount(Planet planet)
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
}

void StatePlanet::OnEnterChannel(ChannelInfo channel)
{
    m_bgm->stop();
    QueueEvent([=] { GetDirector()->SetScene(new StateRoom()); } );
}
