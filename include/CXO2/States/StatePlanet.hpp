#pragma once

#include <CXO2/States/State.hpp>
#include <CXO2/Models/Planet.hpp>

#include <Genode/Audio/AudioMixer.hpp>

namespace Cx
{
    enum class AuthResult : std::uint32_t;

    struct ChannelLoginResponse;
    struct ChannelListResponse;

    class SessionContext;
    class MessageService;
    class AuthService;
    class PlanetService;
    class StatePlanet : public State
    {
    public:
        explicit StatePlanet(
            Gx::AudioMixer& mixer,
            SessionContext& session,
            MessageService& messages,
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

        Gx::AudioMixer& m_mixer;
        MessageService& m_messages;
        AuthService& m_auth;
        PlanetService& m_service;
        SessionContext& m_session;
    };
}
