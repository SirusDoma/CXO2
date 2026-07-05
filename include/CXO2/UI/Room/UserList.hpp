#pragma once

#include <CXO2/Models/Character.hpp>
#include <Genode/UI/UiContainer.hpp>

#include <vector>

namespace Cx
{
    class ChannelService;
    class UserList : public Gx::UiContainer
    {
    public:
        explicit UserList(ChannelService& service);
        void Initialize() override;

        void AddUser(const CharacterInfo& user);
        void Clear();

        void Invalidate() override;

    private:
        void OnRefreshButtonClicked();

        bool m_refreshing{false};

        ChannelService& m_service;
        std::vector<CharacterInfo> m_users;
        std::string m_selectedUser;
        unsigned int m_page = 1;
    };
}
