#ifndef STATE_ROOM_HPP
#define STATE_ROOM_HPP

#include <SFML/Audio.hpp>

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <O2/States/State.hpp>
#include <O2/Data/Planet.hpp>

class StateRoom : public State
{
public:
    virtual void Initialize();

private:
    sf::Music *m_bgm;
};

#endif
