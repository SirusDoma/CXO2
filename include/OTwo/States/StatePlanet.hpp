#ifndef O2JAM_STATE_PLANET_HPP
#define O2JAM_STATE_PLANET_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/Models/Planet.hpp>

class SessionContext;
class StatePlanet : public State
{
public:
    explicit StatePlanet(Gx::Mixer& mixer, SessionContext& session);
    void Initialize() override;

private:
    bool IsConnecting() const;

    void OnMusicHallSelected(MusicHall hall);
    void OnChannelEnter(MusicHall hall, const ServerChannel& channel);

    Gx::Mixer& m_mixer;
    SessionContext& m_session;

    bool m_connecting;
};


#endif
