#ifndef O2JAM_STATE_TEST_HPP
#define O2JAM_STATE_TEST_HPP

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class StateTest : public State
{
public:
    StateTest() = default;
    StateTest(State &state);

    virtual void Initialize();
    virtual bool Close(bool quit = false);
};

#endif