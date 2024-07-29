#ifndef O2JAM_STATE_PLANET_HPP
#define O2JAM_STATE_PLANET_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/Models/Planet.hpp>

class StatePlanet : public State
{
public:
    explicit StatePlanet(State &&state);
    void Initialize() override;

private:
    bool IsConnecting() const;

    void OnMusicHallSelected(MusicHall hall);
    void OnChannelEnter(MusicHall hall, Channel channel);

    bool m_connecting;
};


#endif
