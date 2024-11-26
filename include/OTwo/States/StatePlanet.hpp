#pragma once

#include <OTwo/States/State.hpp>
#include <OTwo/Models/Planet.hpp>
#include <Genode/Audio/AudioMixer.hpp>

class SessionContext;
class StatePlanet : public State
{
public:
    explicit StatePlanet(Gx::AudioMixer& mixer, SessionContext& session);
    void Initialize() override;

private:
    bool IsConnecting() const;

    void OnMusicHallSelected(MusicHall hall);
    void OnChannelEnter(MusicHall hall, const ServerChannel& channel);

    Gx::AudioMixer& m_mixer;
    SessionContext& m_session;

    bool m_connecting;
};
