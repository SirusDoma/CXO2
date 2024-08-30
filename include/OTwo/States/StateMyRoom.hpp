#ifndef O2JAM_STATE_MY_ROOM_HPP
#define O2JAM_STATE_MY_ROOM_HPP


#include <OTwo/States/State.hpp>

#include <OTwo/Avatar/Avatar.hpp>
#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Models/Room.hpp>

class StateMyRoom : public State
{
public:
    StateMyRoom() = default;
    explicit StateMyRoom(State &&state);

    void Initialize() override;

private:
    void InvalidateBagList();

    unsigned int m_bagCurrentPage = 0;
    unsigned int m_bagMaxPage     = 0;
    std::vector<Item*> m_inventory;
};
#endif
