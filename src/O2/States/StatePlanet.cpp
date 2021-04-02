#include <O2/States/StatePlanet.hpp>

#include <Genode/System/Application.hpp>
#include <Genode/Tasks.hpp>
#include <Genode/Fx.hpp>
#include <Genode/UI.hpp>

#include <O2/States/StateRoom.hpp>

StatePlanet::StatePlanet(bool fadeIn) :
    State(),
    m_useFadeIn(fadeIn),
    m_connecting(false)
{

}

void StatePlanet::Initialize()
{
    State::Initialize();

    auto background = Create<Gx::Sprite>("Metadata/State/Planet/Background.json");
    AddChild(background);

    auto tower = Create<Gx::Animation>("Metadata/State/Planet/Tower.json");
    AddChild(tower);

    auto exitButton = Create<Gx::Button>("Metadata/State/Planet/Btn_Exit.json");
    exitButton->SetClickCallback([=] (auto& sender, auto& ev) { GetDirector().GetApplication().Close(); });
    AddChild(exitButton);

    m_dialogInfo = Create<Gx::Dialog>("Metadata/Dialog/Information.json");

    auto philix   = Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Philix.json");
    auto kleo     = Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Kleo.json");
    auto kaliope  = Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Kaliope.json");
    auto euta     = Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Euta.json");
    auto thalo    = Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Thalo.json");
    auto melpomin = Create<Gx::RadioButton>("Metadata/State/Planet/Btn_Melpomin.json");

    m_container = std::make_unique<Gx::UiContainer>();
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
    auto hoverSfx = Create<sf::Sound>("Metadata/State/Planet/Sound/Hover.json", Gx::ResourceScope::Shared);
    for (auto [planet, radio] : planets)
    {
        radio->SetFocusChangedCallback([this, sfx = hoverSfx] (auto &sender, auto &ev)
        {
            auto radio = dynamic_cast<Gx::RadioButton*>(&sender);
            if (!radio || !radio->IsFocused() || radio->IsChecked())
                return;

            Mixer::Play(sfx);
        });

        radio->SetClickCallback([this, sfx = clickSfx, planet = planet] (auto& sender, auto& ev)
        {
            auto radio = dynamic_cast<Gx::RadioButton*>(&sender);
            if (!radio)
                return;

            if (radio->IsChecked() || IsConnecting())
            {
                ev.Handled = true;
                return;
            }

            Mixer::Play(sfx, "sfx");
            m_channelBoard->Show(planet, [=] { OnEnterPlanet(planet); });
        });
    }

    AddChild(m_container.get());

    m_channelBoard = std::make_unique<ChannelBoard>(*this);
    m_channelBoard->SetEnterChannelCallback([=] (auto channel) { OnEnterChannel(channel); });
    AddChild(m_channelBoard.get());

    m_bgm = Create<sf::Music>("Metadata/State/Planet/Music.json", Gx::ResourceScope::Shared);
    Mixer::Play(m_bgm, "BGM");

    if (m_useFadeIn)
    {
        // Overlay fade in animation
        auto overlay = new Gx::Rectangle(sf::Vector2f(800, 600));
        overlay->SetFillColor(sf::Color::White);
        AddChild(overlay);

        Run(new Gx::Sequence([=] { RemoveChild(overlay); }, {
            new Gx::Fade(overlay, 0, sf::seconds(2.5f))
        }));
    }
}

void StatePlanet::OnEnterPlanet(Planet planet)
{
    m_connecting = true;

    auto planetInfo = PlanetInfo();
    planetInfo.Planet = planet;

    for (int x = 0; x < 2; x++)
    {
        for (int i = 1; i <= 20; i++)
        {
            auto channel = ChannelInfo();
            channel.Population = static_cast<int>((i / 20.f) * 100.f);

            planetInfo.Channels.push_back(channel);
        }
    }

    m_connecting = false;
    m_channelBoard->UpdateChannelList(planetInfo);
}

void StatePlanet::OnEnterChannel(ChannelInfo channel)
{
    if (channel.Population >= channel.MaxPopulation)
    {
        if (m_dialogInfo)
            m_dialogInfo->Show(this, "Channel is full");

        return;
    }

    m_connecting = true;
    QueueEvent([=] { GetDirector().SetScene(new StateRoom()); } );
}

bool StatePlanet::IsConnecting()
{
    return m_channelBoard->InTransition() || m_connecting;
}
