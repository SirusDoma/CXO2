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
    void Initialize(Gx::Scene &scene);

    void AddPlayer(Room::PlayerInfo player);
    void Clear();

private:
    constexpr static const unsigned int USER_PER_PAGE = 10;

    virtual void Invalidate();

    Gx::List  *m_list;
    Gx::Label *m_userCountLabel;
    std::vector<Room::PlayerInfo> m_users;

    unsigned int m_selectedUser;
    unsigned int m_page = 1;
};

#endif
