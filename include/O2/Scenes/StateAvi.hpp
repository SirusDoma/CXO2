#ifndef STATE_LOGO_HPP
#define STATE_LOGO_HPP

#include <Genode/SceneGraph.hpp>
#include <O2/Scenes/State.hpp>

class StateAvi : public Gx::Scene
{
public:
    virtual void Initialize();
    virtual bool Close(bool quit = false);
};

#endif