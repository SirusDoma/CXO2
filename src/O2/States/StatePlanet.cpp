#include <O2/States/StatePlanet.hpp>

#include <Genode/System/Application.hpp>
#include <Genode/Tasks.hpp>
#include <Genode/Fx.hpp>
#include <Genode/UI.hpp>

#include <O2/States/StateRoom.hpp>

StatePlanet::StatePlanet(bool fadeIn) :
    State(),
    m_useFadeIn(fadeIn),
    m_connecting(false),
    m_container()
{
}

void StatePlanet::Initialize()
{
    State::Initialize();

    auto& app   = GetApplication();
    auto& mixer = app.Require<Gx::Mixer>();

    auto background = Create<Gx::Sprite>("Interface/Metadata/State/Planet/Background.json");
    AddChild(background);

    auto tower = Create<Gx::Animation>("Interface/Metadata/State/Planet/Tower.json");
    AddChild(tower);

    auto exitButton = Create<Gx::Button>("Interface/Metadata/State/Planet/Btn_Exit.json");
    exitButton->SetClickCallback([=] (auto& sender, auto& ev) { GetDirector().GetApplication().Close(); });
    AddChild(exitButton);

    m_dialogInfo = Create<Gx::Dialog>("Interface/Metadata/Dialog/Information.json");

    auto philix   = Create<Gx::RadioButton>("Interface/Metadata/State/Planet/Btn_Philix.json");
    auto kleo     = Create<Gx::RadioButton>("Interface/Metadata/State/Planet/Btn_Kleo.json");
    auto kalliope = Create<Gx::RadioButton>("Interface/Metadata/State/Planet/Btn_Kaliope.json");
    auto euta     = Create<Gx::RadioButton>("Interface/Metadata/State/Planet/Btn_Euta.json");
    auto thalo    = Create<Gx::RadioButton>("Interface/Metadata/State/Planet/Btn_Thalo.json");
    auto melpomin = Create<Gx::RadioButton>("Interface/Metadata/State/Planet/Btn_Melpomin.json");

    m_container = Gx::UiContainer();
    m_container.AddChild(philix, kleo, kalliope, euta, thalo, melpomin);
    std::unordered_map<Planet::MusicHall, Gx::RadioButton*> planets = {
        {Planet::MusicHall::Melpomin, melpomin},
        {Planet::MusicHall::Thalo,    thalo},
        {Planet::MusicHall::Euta,     euta},
        {Planet::MusicHall::Kalliope, kalliope},
        {Planet::MusicHall::Kleo,     kleo},
        {Planet::MusicHall::Philix,   philix}
    };

    auto clickSfx = mixer.Create<sf::Sound>("Interface/Metadata/State/Planet/Sound/Click.json");
    auto hoverSfx = mixer.Create<sf::Sound>("Interface/Metadata/State/Planet/Sound/Hover.json");
    for (auto [hall, radio] : planets)
    {
        radio->SetFocusChangedCallback([&, sfx = hoverSfx] (auto &sender, auto &ev)
        {
            auto radio = dynamic_cast<Gx::RadioButton*>(&sender);
            if (!radio || !radio->IsFocused() || radio->IsChecked())
                return;

            mixer.Play(sfx, "SFX");
        });

        radio->SetClickCallback([&, sfx = clickSfx, hall = hall] (auto& sender, auto& ev)
        {
            auto radio = dynamic_cast<Gx::RadioButton*>(&sender);
            if (!radio)
                return;

            if (radio->IsChecked() || IsConnecting())
            {
                ev.Handled = true;
                return;
            }

            mixer.Play(sfx, "SFX");
            m_channelBoard->Show(hall, [=] { OnEnterPlanet(hall); });
        });
    }

    AddChild(&m_container);

    m_channelBoard = Create<ChannelBoard>("Interface/Metadata/State/Planet/ChannelBoard.json");
    m_channelBoard->Initialize(*this);
    m_channelBoard->SetEnterChannelCallback([=] (auto hall, auto channel) { OnEnterChannel(hall, channel); });
    AddChild(m_channelBoard);

    m_bgm = mixer.Create<sf::Music>("Interface/Metadata/State/Planet/Music.json");
    mixer.Play(m_bgm, "BGM");

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

void StatePlanet::OnEnterPlanet(Planet::MusicHall hall)
{
    m_connecting = true;

    auto planetInfo = Planet::PlanetInfo();
    planetInfo.Hall = hall;

    for (int x = 0; x < 2; x++)
    {
        for (int i = 1; i <= 20; i++)
        {
            auto channel       = Planet::ChannelInfo();
            channel.Number     = (x * 20) + i;
            channel.Population = static_cast<int>((i / 20.f) * 100.f);

            planetInfo.Channels.push_back(channel);
        }
    }

    m_connecting = false;
    m_channelBoard->UpdateChannelList(planetInfo);
}

void StatePlanet::OnEnterChannel(Planet::MusicHall hall, Planet::ChannelInfo channel)
{
    if (channel.Population >= channel.MaxPopulation)
    {
        if (m_dialogInfo)
            m_dialogInfo->Show(this, "Channel is full");

        return;
    }

    m_connecting = true;
    QueueSceneEvent([=]
    {
        GetApplication().Require<Gx::Mixer>().Stop("BGM");
        GetDirector().SetScene(new StateRoom(hall, channel));
    });
}

bool StatePlanet::IsConnecting()
{
    return m_channelBoard->InTransition() || m_connecting;
}
