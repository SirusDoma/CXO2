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

    auto background = Create<Gx::Sprite>("Metadata/State/Planet/Background.json");
    AddChild(background);

    auto tower = Create<Gx::Animation>("Metadata/State/Planet/Tower.json");
    AddChild(tower);

    auto exitButton = Create<Gx::Button>("Metadata/State/Planet/Btn_Exit.json");
    exitButton->SetClickCallback([=] (auto _) { GetDirector().GetApplication().Close(); });
    AddChild(exitButton);

    auto philix   = Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Philix.json");
    auto kleo     = Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Kleo.json");
    auto kaliope  = Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Kaliope.json");
    auto euta     = Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Euta.json");
    auto thalo    = Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Thalo.json");
    auto melpomin = Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Melpomin.json");

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

    auto clickSfx = Create<sf::Sound>("Metadata/State/Planet/Sound/Click.json", Gx::ResourceScope::Shared);
    for (auto [planet, radio] : planets)
    {
        radio->SetCheckStateChangeCallback([this, sfx = clickSfx, p = planet] (auto sender) {
            if (!sender->IsChecked())
                return;

            Mixer::Play(sfx, "sfx");
            m_container->SetEnabled(false);
            m_channelBoard->Show(p, [=] { OnEnterPlanet(p); });
        });
    }

    AddChild(m_container);

    m_channelBoard = new ChannelBoard(*this);
    m_channelBoard->SetEnterChannelCallback([=] (auto channel) { OnEnterChannel(channel); });
    AddChild(m_channelBoard);

    // Overlay fade in animation
    auto overlay = new Gx::Rectangle(sf::Vector2f(800, 600));
    overlay->SetFillColor(sf::Color::White);
    AddChild(overlay);

    m_bgm = Create<sf::Music>("Metadata/State/Planet/Music.json", Gx::ResourceScope::Shared);
    Run(new Gx::Sequence([=] { RemoveChild(overlay); }, {
        new Gx::Action([this] {  Mixer::Play(m_bgm, "BGM"); }),
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
    QueueEvent([=] { GetDirector().SetScene(new StateRoom()); } );
}
