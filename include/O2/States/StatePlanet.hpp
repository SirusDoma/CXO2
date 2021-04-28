#ifndef O2JAM_STATE_PLANET_HPP
#define O2JAM_STATE_PLANET_HPP

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <O2/States/State.hpp>
#include <O2/Data/Planet.hpp>

#include <O2/States/Components/Planet/ChannelBoard.hpp>

class StatePlanet : public State
{
public:
    StatePlanet(bool fadeIn = true);
    virtual void Initialize();

private:
    bool IsConnecting();

    void OnEnterPlanet(Planet::MusicHall hall);
    void OnEnterChannel(Planet::MusicHall hall, Planet::ChannelInfo channel);

    ChannelBoard   *m_channelBoard;
    Gx::UiContainer m_container;
    sf::Music      *m_bgm;
    Gx::Dialog     *m_dialogInfo;
    bool            m_useFadeIn;
    bool            m_connecting;
};

#endif
