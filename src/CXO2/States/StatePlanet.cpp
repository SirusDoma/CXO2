#include <CXO2/States/StatePlanet.hpp>
#include <CXO2/States/StateRoom.hpp>

#include <CXO2/O2.hpp>
#include <CXO2/Contexts/SessionContext.hpp>

#include <CXO2/Services/AuthService.hpp>
#include <CXO2/Network/Requests/AuthRequest.hpp>
#include <CXO2/Network/Requests/PingRequest.hpp>
#include <CXO2/Network/Responses/AuthResponse.hpp>
#include <CXO2/Network/Responses/PingResponse.hpp>

#include <CXO2/Models/Planet.hpp>
#include <CXO2/Services/NetworkService.hpp>
#include <CXO2/UI/Planet/ChannelBoard.hpp>

#include <CXO2/Constants/Identifiers/Sound.hpp>
#include <CXO2/Constants/Identifiers/Planet.hpp>
#include <CXO2/Constants/Messages/Network.hpp>
#include <CXO2/Constants/Messages/Planet.hpp>

#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Tween/Fade.hpp>

namespace Cx
{
    using namespace Constants::Identifiers;

    class AuthService;
    class PlanetService;
    StatePlanet::StatePlanet(
        Gx::AudioMixer& mixer,
        SessionContext& session,
        NetworkService& network,
        AuthService& auth,
        PlanetService& service
    ) :
        m_mixer(mixer),
        m_network(network),
        m_auth(auth),
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
        exitButton->SetClickCallback([&] (auto&, auto&) { ExitGame(Constants::Messages::Planet::EXIT_CONFIRM); });

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

            radio->SetCheckStateChangeCallback([&, channelBoard, hall = musicHall, clickSfx] (auto& sender, auto& ev)
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
                    message = Constants::Messages::Planet::Authentication::DATABASE_ERROR;
                    break;
                case AuthResult::Banned:
                    message = Constants::Messages::Planet::Authentication::BANNED;
                    break;
                case AuthResult::DuplicateSessions:
                    message = Constants::Messages::Planet::Authentication::CONNECTING;
                    break;
                case AuthResult::InvalidCredentials:
                    message = Constants::Messages::Planet::Authentication::INVALID_CREDENTIALS;
                    break;
                case AuthResult::IllegalUser:
                    message = Constants::Messages::Planet::Authentication::ILLEGAL_USER;
                    break;
                case AuthResult::InsufficientBalance:
                    message = Constants::Messages::Planet::Authentication::INSUFFICIENT_POINTS;
                    break;
                case AuthResult::MultiGamesSession:
                    message = Constants::Messages::Planet::Authentication::ALREADY_CONNECTED;
                    break;
                default:
                    message = Constants::Messages::Planet::Authentication::NETWORK_ERROR;
                    break;
            }

            ShowDialog(message, DialogStyle::Information);

            const auto container = Instantiate<Gx::UiContainer>(Resource::Planet::IDC_CONTAINER_MUSIC_HALL);
            container->SetEnabled(true);

            return;
        }

        m_service.GetChannelList([=] (const auto& ev)
        {
            try
            {
                OnChannelListUpdated(ev.Open());
            }
            catch (const Gx::NetworkException&)
            {
                const auto container = Instantiate<Gx::UiContainer>(Resource::Planet::IDC_CONTAINER_MUSIC_HALL);
                container->SetEnabled(true);

                ShowDialog(Constants::Messages::Network::CONNECT_FAILED, DialogStyle::Information);
            }
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
            ShowDialog(Constants::Messages::Planet::CHANNEL_FULL, DialogStyle::Information);
            return;
        }

        m_network.StartHeartbeat<PingRequest, PingResponse>(sf::seconds(10), [] (const auto&)
        {
            auto state = dynamic_cast<State*>(&Gx::Application::Instance().GetModule<Gx::SceneDirector>().GetPresentingScene());
            state->ShowDialog(Constants::Messages::Network::UNSTABLE, DialogStyle::Information, false, [] (bool)
            {
                Gx::Application::Instance().GetModule<Gx::SceneDirector>().Present<StatePlanet>();
            });
        });

        auto& director = GetDirector();
        director.Present<StateRoom>();
    }

    void StatePlanet::OnMusicHallSelected(const MusicHall hall)
    {
        const auto container = Instantiate<Gx::UiContainer>(Resource::Planet::IDC_CONTAINER_MUSIC_HALL);
        container->SetEnabled(false);

        const auto channelBoard = Instantiate<ChannelBoard>(Resource::Planet::IDC_CHANNEL_BOARD);
        channelBoard->SetEnabled(false);

        m_auth.Authenticate(hall, AuthRequest{m_session.GetToken()},
        [=] (const auto& ev)
        {
            try
            {
                const auto& response = ev.Open();
                m_session.SetMusicHall(hall);
                OnAuthenticated(response.ResultCode);
            }
            catch (const Gx::NetworkException&)
            {
                container->SetEnabled(true);
                ShowDialog(Constants::Messages::Network::CONNECT_FAILED, DialogStyle::Information);
            }
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
        [=] (const auto& ev)
        {
            try
            {
                OnChannelLogin(ev.Open());
            }
            catch (const Gx::NetworkException&)
            {
                ShowDialog(Constants::Messages::Network::CONNECT_FAILED, DialogStyle::Information);
                container->SetEnabled(true);
                channelBoard->SetEnabled(true);
            }
        });
    }
}
