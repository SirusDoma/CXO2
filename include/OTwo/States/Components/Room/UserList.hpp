#ifndef O2JAM_ROOM_USER_LIST_HPP
#define O2JAM_ROOM_USER_LIST_HPP

#include <Genode/UI/UiContainer.hpp>

#include <Genode/UI/List.hpp>
#include <Genode/UI/Label.hpp>

#include <OTwo/Data/Room.hpp>

#include <vector>

class UserList : public Gx::UiContainer
{
public:
    UserList();
    void Initialize() override;

    void AddUser(Player user);
    void Clear();

private:
    virtual void Invalidate();
    std::vector<Player> m_users;

    unsigned int m_selectedUser;
    unsigned int m_page = 1;
};

#endif
