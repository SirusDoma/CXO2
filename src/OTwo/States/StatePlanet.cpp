#include <OTwo/States/StatePlanet.hpp>
#include <OTwo/UI/Planet/ChannelBoard.hpp>
#include <OTwo/Models/Planet.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/States/StateRoom.hpp>

#include <Genode/System/Application.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Tween/Fade.hpp>

StatePlanet::StatePlanet(Gx::Mixer& mixer, SessionContext& session) :
    m_mixer(mixer),
    m_session(session),
    m_connecting(false)
{
}

void StatePlanet::Initialize()
{
    State::Initialize();

    const auto bgm = Instantiate<sf::Music>("BGM/bgMainRoom.ogg");
    auto clickSfx  = Instantiate<sf::Sound>("bgEffect/02");
    auto hoverSfx  = Instantiate<sf::Sound>("Planet/BEEppq");

    const auto container = Instantiate<Gx::UiContainer>("IDC_CONTAINER_MUSIC_HALL");
    auto euta     = container->FindChild<Gx::RadioButton>("IDC_RADIO_BEGINNER_01");
    auto thalo    = container->FindChild<Gx::RadioButton>("IDC_RADIO_BEGINNER_02");
    auto melpomin = container->FindChild<Gx::RadioButton>("IDC_RADIO_BEGINNER_03");
    auto kalliope = container->FindChild<Gx::RadioButton>("IDC_RADIO_HIGH_01");
    auto kleo     = container->FindChild<Gx::RadioButton>("IDC_RADIO_INTERMEDIATE_01");
    auto philix   = container->FindChild<Gx::RadioButton>("IDC_RADIO_PREMIUM_01");

    const auto exitButton = Instantiate<Gx::Button>("IDC_BUTTON_EXIT");
    exitButton->SetClickCallback([&] (auto& sender, auto& ev) { GetApplication().Close(); });

    auto channelBoard = Instantiate<ChannelBoard>("IDC_CHANNEL_BOARD");
    channelBoard->SetChannelEnterCallback([=] (auto hall, auto channel) { OnChannelEnter(hall, channel); });

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
        radio->SetFocusChangedCallback([&, hoverSfx] (auto& sender, auto& ev)
        {
            if (const auto r = dynamic_cast<Gx::RadioButton*>(&sender); !r || !r->IsFocused() || r->IsChecked())
                return;

            m_mixer.Play(hoverSfx, "SFX");
        });

        radio->SetCheckStateChangeCallback([&, channelBoard, hall = musicHall, clickSfx] (auto& sender)
        {
            if (!sender.IsChecked() || channelBoard->InTransition() || IsConnecting())
                return;

            m_mixer.Play(clickSfx, "SFX");
            channelBoard->Show(hall, [=] { OnMusicHallSelected(hall); });
        });
    }

    if (m_session.GetMusicHall() == MusicHall::None)
    {
        auto& overlay = Create<Gx::Rectangle>(GetView().getSize());
        overlay.SetFillColor(sf::Color::Black);
        AddChild(overlay);

        auto& transition = Create<Gx::Sequence>([&]
            {
                RemoveChild(overlay);
            },
            Create<Gx::Fade>(overlay, 0, sf::seconds(1.0f))
        );
        Run(transition);
    }

    bgm->setLooping(true);
    m_mixer.Play(bgm, "BGM");
}

bool StatePlanet::IsConnecting() const
{
    return m_connecting;
}

void StatePlanet::OnMusicHallSelected(const MusicHall hall)
{
    m_connecting = true;
    const auto channelBoard = Instantiate<ChannelBoard>("IDC_CHANNEL_BOARD");

    auto planetInfo = PlanetInfo();
    planetInfo.Hall = hall;

    for (unsigned int x = 0; x < 2; x++)
    {
        for (unsigned int i = 1; i <= 20; i++)
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

void StatePlanet::OnChannelEnter(const MusicHall hall, const ServerChannel& channel)
{
    if (channel.Population >= channel.MaxPopulation)
    {
        ShowDialog("Channel is full.", DialogStyle::Information, false, [] (auto _) {});
        return;
    }

    m_session.SetMusicHall(hall);
    m_session.SetChannelID(channel.ID);

    m_connecting = true;
    auto& sequence = Create<Gx::Sequence>([&]
        {
            auto& director = GetDirector();
            director.Present<StateRoom>();
        },
        Create<Gx::Delay>(sf::milliseconds(750))
    );

    Run(sequence);
}
