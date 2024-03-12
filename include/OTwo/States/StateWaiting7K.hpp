#ifndef O2JAM_STATE_WAITING_7K_HPP
#define O2JAM_STATE_WAITING_7K_HPP

#include <OTwo/States/State.hpp>

class StateWaiting7K : public State
{
public:
    StateWaiting7K(State& state);

    virtual void Initialize();
};

#endif
