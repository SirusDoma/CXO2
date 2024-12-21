#pragma once

#include <Genode/UI/UiContainer.hpp>

#include <Genode/UI/List.hpp>
#include <Genode/UI/Label.hpp>

#include <OTwo/Models/Room.hpp>

#include <vector>

class UserList : public Gx::UiContainer
{
public:
    UserList();
    void Initialize() override;

    void AddUser(const Player& user);
    void Clear();

private:
    void Invalidate() override;
    std::vector<Player> m_users;

    unsigned int m_selectedUser;
    unsigned int m_page = 1;
};
