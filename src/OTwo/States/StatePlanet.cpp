#include <OTwo/States/StatePlanet.hpp>
#include <OTwo/UI/Planet/ChannelBoard.hpp>
#include <OTwo/Models/Planet.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/States/StateTest.hpp>
#include <OTwo/States/StateRoom.hpp>

#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Fx/Fade.hpp>

StatePlanet::StatePlanet(State &&state) :
    State(std::move(state)),
    m_connecting(false)
{
}

void StatePlanet::Initialize()
{
    State::Initialize();

    const auto& session = Require<SessionContext>();
    auto& mixer         = Require<Gx::Mixer>();

    const auto bgm = Instantiate<sf::Music>("STATE_PLANET/IDC_MUSIC");
    auto clickSfx  = Instantiate<sf::Sound>("STATE_PLANET/IDC_SOUND_02");
    auto hoverSfx  = Instantiate<sf::Sound>("STATE_PLANET/IDC_SOUND_BEEP");

    auto euta     = Instantiate<Gx::RadioButton>("STATE_PLANET/IDC_RADIO_BEGINNER_01");
    auto thalo    = Instantiate<Gx::RadioButton>("STATE_PLANET/IDC_RADIO_BEGINNER_02");
    auto melpomin = Instantiate<Gx::RadioButton>("STATE_PLANET/IDC_RADIO_BEGINNER_03");
    auto kalliope = Instantiate<Gx::RadioButton>("STATE_PLANET/IDC_RADIO_HIGH_01");
    auto kleo     = Instantiate<Gx::RadioButton>("STATE_PLANET/IDC_RADIO_INTERMEDIATE_01");
    auto philix   = Instantiate<Gx::RadioButton>("STATE_PLANET/IDC_RADIO_PREMIUM_01");

    auto channelBoard = Instantiate<ChannelBoard>("STATE_PLANET/IDC_CHANNEL_BOARD");
    channelBoard->Initialize();
    channelBoard->SetChannelEnterCallback([=] (auto hall, auto channel) { OnChannelEnter(hall, channel); });

    const auto exitButton = Instantiate<Gx::Button>("STATE_PLANET/IDC_BUTTON_EXIT");
    exitButton->SetClickCallback([&] (auto& sender, auto& ev) { GetApplication().Close(); });

    const auto container = Create<Gx::UiContainer>();
    container->SetName("STATE_PLANET/IDC_CONTAINER_MUSIC_HALL");
    container->AddChild(philix, kleo, kalliope, euta, thalo, melpomin);
    AddChild(container);

    std::unordered_map<MusicHall, Gx::RadioButton*> planets =
    {
        { MusicHall::Melpomin, melpomin },
        { MusicHall::Thalo,    thalo    },
        { MusicHall::Euta,     euta     },
        { MusicHall::Kalliope, kalliope },
        { MusicHall::Kleo,     kleo     },
        { MusicHall::Philix,   philix   }
    };

    for (auto [musicHall, radio] : planets)
    {
        radio->SetFocusChangedCallback([&, hoverSfx] (auto &sender, auto &ev)
        {
            if (const auto r = dynamic_cast<Gx::RadioButton*>(&sender); !r || !r->IsFocused() || r->IsChecked())
                return;

            mixer.Play(hoverSfx, "SFX");
        });

        radio->SetClickCallback([&, channelBoard, hall = musicHall, clickSfx] (auto& sender, auto& ev)
        {
            const auto r = dynamic_cast<Gx::RadioButton*>(&sender);
            if (!r)
                return;

            if (r->IsChecked() || channelBoard->InTransition() || IsConnecting())
            {
                ev.Handled = true;
                return;
            }

            mixer.Play(clickSfx, "SFX");
            channelBoard->Show(hall, [=] { OnMusicHallSelected(hall); });
        });
    }

    if (session.GetMusicHall() == MusicHall::None)
    {
        auto overlay = Create<Gx::Rectangle>(GetView().getSize());
        overlay->SetFillColor(sf::Color::Black);
        AddChild(overlay);

        const auto splash = Create<Gx::Sequence>([&, overlay]
            {
                RemoveChild(overlay);
            },
            Gx::Sequence::ListOf({
                Create<Gx::Fade>(overlay, 0, sf::seconds(2.5f))
            })
        );
        Run(splash);
    }

    mixer.Play(bgm, "BGM");
}

bool StatePlanet::IsConnecting() const
{
    return m_connecting;
}

void StatePlanet::OnMusicHallSelected(MusicHall hall)
{
    m_connecting = true;
    const auto channelBoard = Instantiate<ChannelBoard>("STATE_PLANET/IDC_CHANNEL_BOARD");

    auto planetInfo = PlanetInfo();
    planetInfo.Hall = hall;

    for (int x = 0; x < 2; x++)
    {
        for (int i = 1; i <= 20; i++)
        {
            auto channel       = ServerChannel();
            channel.ID         = (x * 20) + i;
            channel.Population = static_cast<int>((i / 20.f) * 100.f);

            planetInfo.Channels.push_back(channel);
        }
    }

    m_connecting = false;
    channelBoard->UpdateChannelList(planetInfo);
}

void StatePlanet::OnChannelEnter(MusicHall hall, ServerChannel channel)
{
    if (channel.Population >= channel.MaxPopulation)
    {
        ShowDialog("Channel is full.", DialogStyle::Information, false, [] (auto _) {});
        return;
    }

    auto& session = Require<SessionContext>();
    session.SetMusicHall(hall);
    session.SetChannelID(channel.ID);

    m_connecting = true;
    const auto sequence = Create<Gx::Sequence>([&] ()
        {
            auto& director = GetDirector();
            director.Present<StateRoom>();
        },
        Gx::Sequence::ListOf({
            Create<Gx::Delay>(sf::milliseconds(750))
        })
    );

    Run(sequence);
}
