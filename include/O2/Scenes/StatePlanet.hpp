#ifndef STATE_PLANET_HPP
#define STATE_PLANET_HPP

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>

class StatePlanet : public Gx::Scene
{
public:
    virtual void Initialize();
};

#endif
