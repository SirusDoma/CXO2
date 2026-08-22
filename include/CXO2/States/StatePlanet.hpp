#pragma once

#include <CXO2/States/State.hpp>
#include <CXO2/Models/Planet.hpp>
#include <CXO2/UI/Components/Planet/ChannelBoard.hpp>

#include <Genode/Audio/AudioMixer.hpp>

namespace Cx
{
    enum class AuthResult : std::uint32_t;

    struct ChannelLoginResponse;
    struct ChannelListResponse;

    class SessionContext;
    class NetworkService;
    class AuthService;
    class PlanetService;
    class StatePlanet : public State
    {
    public:
        explicit StatePlanet(
            Gx::AudioMixer& mixer,
            SessionContext& session,
            NetworkService& network,
            AuthService& auth,
            PlanetService& service
        );

        void Initialize() override;

        void OnAuthenticated(AuthResult result);
        void OnChannelListUpdated(const ChannelListResponse& response);
        void OnChannelLogin(const ChannelLoginResponse& response);

    private:
        void OnMusicHallSelected(MusicHall hall);
        void OnChannelEnterButtonClicked(MusicHall hall, std::uint16_t serverID, std::uint16_t channelID);
        void OnChannelBoardTabChanged(ChannelBoard& sender, ChannelBoard::TabChangedEvent& ev);

        Gx::AudioMixer& m_mixer;
        NetworkService& m_network;
        AuthService& m_auth;
        PlanetService& m_service;
        SessionContext& m_session;
    };
}
