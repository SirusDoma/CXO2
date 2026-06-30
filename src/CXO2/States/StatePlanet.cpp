#include <CXO2/States/StatePlanet.hpp>
#include <CXO2/States/StateRoom.hpp>

#include <CXO2/O2.hpp>
#include <CXO2/Contexts/SessionContext.hpp>

#include <CXO2/Network/NetworkAdapter.hpp>
#include <CXO2/Messages/Requests/AuthRequest.hpp>
#include <CXO2/Messages/Responses/AuthResponse.hpp>

#include <CXO2/Models/Planet.hpp>
#include <CXO2/UI/Planet/ChannelBoard.hpp>

#include <CXO2/StringTable/Identifiers/Sound.hpp>
#include <CXO2/StringTable/Identifiers/Planet.hpp>

#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Tween/Fade.hpp>
#include <CXO2/Services/AuthService.hpp>
#include <CXO2/Services/NetworkService.hpp>

namespace Cx
{
    using namespace StringTable::Identifiers;

    class AuthService;
    class PlanetService;
    StatePlanet::StatePlanet(Gx::AudioMixer& mixer, SessionContext& session, NetworkService& network, AuthService& auth, PlanetService& service) :
        m_mixer(mixer),
        m_auth(auth),
        m_network(network),
        m_service(service),
        m_session(session)
    {
    }

    void StatePlanet::Initialize()
    {
        State::Initialize();
        m_network.StopHeartbeat();

        const auto bgm = Instantiate<sf::Music>(Sound::BGM::BG_MAIN_ROOM);
        auto clickSfx  = Instantiate<sf::Sound>(Sound::Effects::EF_02);
        auto hoverSfx  = Find<sf::Sound>(Sound::Effects::PLANET_BEEP);

        const auto container = Instantiate<Gx::UiContainer>(Resource::Planet::IDC_CONTAINER_MUSIC_HALL);
        auto euta     = container->FindChild<Gx::RadioButton>(Resource::Planet::IDC_RADIO_BEGINNER_01);
        auto thalo    = container->FindChild<Gx::RadioButton>(Resource::Planet::IDC_RADIO_BEGINNER_02);
        auto melpomin = container->FindChild<Gx::RadioButton>(Resource::Planet::IDC_RADIO_BEGINNER_03);
        auto kalliope = container->FindChild<Gx::RadioButton>(Resource::Planet::IDC_RADIO_HIGH_01);
        auto kleo     = container->FindChild<Gx::RadioButton>(Resource::Planet::IDC_RADIO_INTERMEDIATE_01);
        auto philix   = container->FindChild<Gx::RadioButton>(Resource::Planet::IDC_RADIO_PREMIUM_01);

        const auto exitButton = Instantiate<Gx::Button>(Resource::Planet::IDC_BUTTON_EXIT);
        exitButton->SetClickCallback([&] (auto&, auto&) { GetApplication().Close(); });

        auto channelBoard = Instantiate<ChannelBoard>(Resource::Planet::IDC_CHANNEL_BOARD);
        channelBoard->SetChannelEnterCallback([=] (auto hall, std::uint16_t serverID, std::uint16_t channelID)
        {
            OnChannelEnterButtonClicked(hall, serverID, channelID);
        });

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
            radio->SetFocusChangedCallback([&, hoverSfx] (auto& sender, auto&)
            {
                if (const auto r = dynamic_cast<Gx::RadioButton*>(&sender); !r || !r->IsFocused() || r->IsChecked())
                    return;

                if (hoverSfx)
                    m_mixer.Play(*hoverSfx, Sound::Channel::SFX);
            });

            radio->SetCheckStateChangeCallback([&, channelBoard, hall = musicHall, clickSfx] (auto& sender)
            {
                if (!sender.IsChecked() || channelBoard->InTransition())
                    return;

                m_mixer.Play(*clickSfx, Sound::Channel::SFX);
                channelBoard->Show(hall, [=] { OnMusicHallSelected(hall); });
            });
        }

        if (m_session.GetMusicHall() == MusicHall::None && !O2::InInteropMode(InteropMode::Interface))
        {
            auto& overlay = Create<Gx::Rectangle>(GetView().getSize());
            overlay.SetColor(sf::Color::Black);
            AddChild(overlay);

            Run<Gx::Sequence>([&]
                {
                    RemoveChild(overlay);
                },
                Gx::Fade(overlay, 0, sf::seconds(1.0f))
            );
        }

        bgm->setLooping(true);
        m_mixer.Play(*bgm, Sound::Channel::BGM);
    }

    void StatePlanet::OnAuthenticated(const AuthResult result)
    {

        if (result != AuthResult::Success)
        {
            auto message = std::string();
            switch (result)
            {
                case AuthResult::DatabaseError:
                    message = "(Please inquire of Administrator.) DB error";
                    break;
                case AuthResult::Banned:
                    message = "You have been banned!\nPlease enquire customer service for detail.";
                    break;
                case AuthResult::DuplicateSessions:
                    message = "User is now being connected to the Game.";
                    break;
                case AuthResult::InvalidCredentials:
                    message = "Either login name or password is incorrect.";
                    break;
                case AuthResult::IllegalUser:
                    message = "You are illegal user.";
                    break;
                case AuthResult::InsufficientBalance:
                    message = "You have insufficient points to play. Please top up";
                    break;
                case AuthResult::MultiGamesSession:
                    message = "You have already connected another game.";
                    break;
                default:
                    message = "Network Error has occurred.";
                    break;
            }

            ShowDialog(message, DialogStyle::Information);

            const auto container = Instantiate<Gx::UiContainer>(Resource::Planet::IDC_CONTAINER_MUSIC_HALL);
            container->SetEnabled(true);

            return;
        }

        m_service.GetChannelList([=] (const ChannelListResponse& response)
        {
            OnChannelListUpdated(response);
        },
        [=](const auto&)
        {
            Invoke([=]
            {
                const auto container = Instantiate<Gx::UiContainer>(Resource::Planet::IDC_CONTAINER_MUSIC_HALL);
                container->SetEnabled(true);

                ShowDialog("Failed in connecting to the server.", DialogStyle::Information);
            });
        });
    }

    void StatePlanet::OnChannelListUpdated(const ChannelListResponse& response)
    {
        const auto container = Instantiate<Gx::UiContainer>(Resource::Planet::IDC_CONTAINER_MUSIC_HALL);
        container->SetEnabled(true);

        const auto channelBoard = Instantiate<ChannelBoard>(Resource::Planet::IDC_CHANNEL_BOARD);
        channelBoard->UpdateChannelList(m_session.GetMusicHall(), response);
        channelBoard->SetEnabled(true);
    }

    void StatePlanet::OnChannelLogin(const ChannelLoginResponse& response)
    {
        if (response.Full)
        {
            ShowDialog("Channel is full.", DialogStyle::Information);
            return;
        }

        m_network.StartHeartbeat();
        Invoke([=]
        {
            auto& director = GetDirector();
            director.Present<StateRoom>();
        });
    }

    void StatePlanet::OnMusicHallSelected(const MusicHall hall)
    {
        const auto container = Instantiate<Gx::UiContainer>(Resource::Planet::IDC_CONTAINER_MUSIC_HALL);
        container->SetEnabled(false);

        const auto channelBoard = Instantiate<ChannelBoard>(Resource::Planet::IDC_CHANNEL_BOARD);
        channelBoard->SetEnabled(false);

        m_network.Start(hall, [=] (const bool success)
        {
            Invoke([=]
            {
                if (!success)
                {
                    container->SetEnabled(true);
                    ShowDialog("Failed in connecting to the server.", DialogStyle::Information);

                    return;
                }

                m_auth.Authenticate(hall, m_session.GetToken(),
                [=] (const auto& response)
                {
                    Invoke([=]
                    {
                        m_session.SetMusicHall(hall);
                        OnAuthenticated(response.ResultCode);
                    });
                },
                [=] (const auto& ex)
                {
                    Invoke([=]
                    {
                        container->SetEnabled(true);
                        ShowDialog("Failed in connecting to the server.", DialogStyle::Information);
                    });
                });
            });
        });
    }

    void StatePlanet::OnChannelEnterButtonClicked(const MusicHall hall, const std::uint16_t serverID, const std::uint16_t channelID)
    {
        const auto container = Instantiate<Gx::UiContainer>(Resource::Planet::IDC_CONTAINER_MUSIC_HALL);
        container->SetEnabled(false);

        const auto channelBoard = Instantiate<ChannelBoard>(Resource::Planet::IDC_CHANNEL_BOARD);
        channelBoard->SetEnabled(false);

        m_session.SetMusicHall(hall);
        m_session.SetChannelID(channelID);

        m_service.Login(ChannelLoginRequest{serverID, channelID},
        [=] (const ChannelLoginResponse& response)
        {
            OnChannelLogin(response);
        },
        [=] (const auto&)
        {
            Invoke([=]
            {
                ShowDialog("Failed in connecting to the server.", DialogStyle::Information);
                container->SetEnabled(true);
                channelBoard->SetEnabled(true);
            });
        });
    }
}
