#include <OTwo/States/StatePlanet.hpp>
#include <OTwo/States/Components/Planet/ChannelBoard.hpp>
#include <OTwo/Data/Planet.hpp>
#include <OTwo/Data/UserState.hpp>
#include <OTwo/States/StateTest.hpp>
#include <OTwo/States/StateRoom.hpp>

#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Fx/Fade.hpp>

StatePlanet::StatePlanet(State &state) :
    State::State(state),
    m_connecting(false)
{
}

void StatePlanet::Initialize()
{
    State::Initialize();

    auto state    = Require<UserState>();
    auto& mixer   = Require<Gx::Mixer>();

    auto bgm      = Load<sf::Music>("STATE_PLANET/IDC_MUSIC");
    auto clickSfx = Load<sf::Sound>("STATE_PLANET/IDC_SOUND_CLICK");
    auto hoverSfx = Load<sf::Sound>("STATE_PLANET/IDC_SOUND_HOVER");

    auto melpomin = Load<Gx::RadioButton>("STATE_PLANET/IDC_RADIO_MELPOMIN");
    auto thalo    = Load<Gx::RadioButton>("STATE_PLANET/IDC_RADIO_THALO");
    auto euta     = Load<Gx::RadioButton>("STATE_PLANET/IDC_RADIO_EUTA");
    auto kalliope = Load<Gx::RadioButton>("STATE_PLANET/IDC_RADIO_KALLIOPE");
    auto kleo     = Load<Gx::RadioButton>("STATE_PLANET/IDC_RADIO_KLEO");
    auto philix   = Load<Gx::RadioButton>("STATE_PLANET/IDC_RADIO_PHILIX");

    auto channelBoard = Load<ChannelBoard>("STATE_PLANET/IDC_CHANNEL_BOARD");
    channelBoard->Initialize();
    channelBoard->SetChannelEnterCallback([=] (auto hall, auto channel) { OnChannelEnter(hall, channel); });

    auto exitButton = Load<Gx::Button>("STATE_PLANET/IDC_BUTTON_EXIT");
    exitButton->SetClickCallback([&] (auto& sender, auto& ev) { GetApplication().Close(); });

    auto container = Make<Gx::UiContainer>("STATE_PLANET/IDC_CONTAINER_MUSIC_HALL");
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
            if (auto r = dynamic_cast<Gx::RadioButton*>(&sender); !r || !r->IsFocused() || r->IsChecked())
                return;

            mixer.Play(hoverSfx, "SFX");
        });

        radio->SetClickCallback([&, channelBoard, hall = musicHall, clickSfx] (auto& sender, auto& ev)
        {
            auto r = dynamic_cast<Gx::RadioButton*>(&sender);
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

    if (state.GetMusicHall() == MusicHall::None)
    {
        auto overlay = Create<Gx::Rectangle>(GetView().getSize());
        auto splash = Create<Gx::Sequence>([&, overlay]
            {
                RemoveChild(overlay);
            },
            Gx::Sequence::ListOf({
                Create<Gx::Fade>(overlay, 0, sf::seconds(2.5f))
            })
        );

        overlay->SetFillColor(sf::Color::White);
        AddChild(overlay);
        Run(splash);
    }

    mixer.Play(bgm, "BGM");
}

bool StatePlanet::IsConnecting()
{
    return m_connecting;
}

void StatePlanet::OnMusicHallSelected(MusicHall hall)
{
    m_connecting = true;
    auto channelBoard = Load<ChannelBoard>("STATE_PLANET/IDC_CHANNEL_BOARD");

    auto planetInfo = Planet();
    planetInfo.Hall = hall;

    for (int x = 0; x < 2; x++)
    {
        for (int i = 1; i <= 20; i++)
        {
            auto channel       = Channel();
            channel.ID         = (x * 20) + i;
            channel.Population = static_cast<int>((i / 20.f) * 100.f);

            planetInfo.Channels.push_back(channel);
        }
    }

    m_connecting = false;
    channelBoard->UpdateChannelList(planetInfo);
}

void StatePlanet::OnChannelEnter(MusicHall hall, Channel channel)
{
//    if (channel.Population >= channel.MaxPopulation)
//    {
//        if (m_dialogInfo)
//            m_dialogInfo->Show(this, "Channel is full");
//
//        return;
//    }

    auto& state = Require<UserState>();
    state.SetMusicHall(hall);
    state.SetChannelID(channel.ID);

    m_connecting = true;
    auto sequence = Create<Gx::Sequence>([&] ()
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
