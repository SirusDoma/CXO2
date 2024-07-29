#ifndef O2JAM_STATE_LOGO_HPP
#define O2JAM_STATE_LOGO_HPP

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class StateAvi : public State
{
public:
    StateAvi() = default;
    explicit StateAvi(State &&state);

    void Initialize() override;
    bool Close(bool quit) override;
};

#endif