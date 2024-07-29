#ifndef O2JAM_STATE_WAITING_7K_HPP
#define O2JAM_STATE_WAITING_7K_HPP

#include <OTwo/States/State.hpp>

class StateWaiting7K : public State
{
public:
    explicit StateWaiting7K(State &&state);

    void Initialize() override;
};

#endif
