#ifndef O2JAM_STATE_PLANET_HPP
#define O2JAM_STATE_PLANET_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/Data/Planet.hpp>

class StatePlanet : public State
{
public:
    StatePlanet(State& state);
    virtual void Initialize();

private:
    bool IsConnecting();

    void OnMusicHallSelected(MusicHall hall);
    void OnChannelEnter(MusicHall hall, Channel channel);

    bool m_connecting;
};


#endif
