#ifndef STATE_PLANET_HPP
#define STATE_PLANET_HPP

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
    void ShowChannelBoard(Planet planet);
    void GetChannelCount(Planet planet);

    ChannelBoard    *m_channelBoard;
    Gx::UiContainer *m_container;
};

#endif
