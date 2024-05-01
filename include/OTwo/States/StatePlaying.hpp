#ifndef O2JAM_STATE_PLAYING_HPP
#define O2JAM_STATE_PLAYING_HPP

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class StatePlaying : public State
{
public:
    StatePlaying() = default;
    explicit StatePlaying(State &state);

    void Initialize() override;
};


#endif
