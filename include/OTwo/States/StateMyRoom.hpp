#ifndef O2JAM_STATE_MY_ROOM_HPP
#define O2JAM_STATE_MY_ROOM_HPP

#include <Genode/SceneGraph.hpp>
#include <OTwo/States/State.hpp>

class StateMyRoom : public State
{
public:
    StateMyRoom() = default;
    explicit StateMyRoom(State &&state);

    void Initialize() override;
};
#endif
