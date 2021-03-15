#ifndef STATE_PLANET_HPP
#define STATE_PLANET_HPP

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Tasks/Sequence.hpp>

enum Planet
{
    Melpomin,
    Thalo,
    Euta,
    Kaliope,
    Kleo,
    Philix
};

class StatePlanet : public Gx::Scene
{
public:
    virtual void Initialize();

private:
    Gx::Sprite *m_channelBoard;
    Gx::Sequence *m_channelBoardSequence;

    void ShowChannelBoard(Planet planet);

};

#endif
