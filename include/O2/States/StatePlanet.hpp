#ifndef STATE_PLANET_HPP
#define STATE_PLANET_HPP

#include <SFML/Audio.hpp>

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <O2/States/State.hpp>
#include <O2/Data/Planet.hpp>

#include <O2/Components/Planet/ChannelBoard.hpp>

class StatePlanet : public State
{
public:
    virtual void Initialize();

private:
    void OnEnterPlanet(Planet planet);
    void OnEnterChannel(ChannelInfo channel);

    ChannelBoard    *m_channelBoard;
    Gx::UiContainer *m_container;
    sf::Music       *m_bgm;
};

#endif
