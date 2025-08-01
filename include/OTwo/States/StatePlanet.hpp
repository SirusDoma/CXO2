#pragma once

#include <OTwo/States/State.hpp>
#include <OTwo/Models/Planet.hpp>

#include <Genode/Audio/AudioMixer.hpp>

#include <future>

enum class AuthResult : std::uint32_t;

struct ChannelLoginResponse;
struct ChannelListResponse;

class SessionContext;
class NetworkAdapter;
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

    Gx::AudioMixer& m_mixer;
    AuthService& m_auth;
    NetworkService& m_network;
    PlanetService& m_service;
    SessionContext& m_session;

    std::future<void> m_task{};
};
