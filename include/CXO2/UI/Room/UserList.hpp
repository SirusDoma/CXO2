#pragma once

#include <Genode/UI/UiContainer.hpp>

#include <CXO2/Network/MessageEnvelope.hpp>
#include <CXO2/Network/Responses/UserListResponse.hpp>

#include <unordered_map>
#include <vector>

namespace Cx
{
    class ChannelService;
    class UserList : public Gx::UiContainer
    {
    public:
        explicit UserList(ChannelService& service);
        void Initialize() override;

        void AddUser(const UserListResponse::User& user);
        void Clear();

        void Invalidate() override;

    private:
        void OnUserListLoad(const MessageEnvelope<UserListResponse>& ev);

        void OnUserButtonCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev);
        void OnUserButtonFocusChanged(Gx::Control& sender, Gx::Control::Event& ev);

        void OnRefreshButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnUserLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnUserRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        bool m_refreshing{false};

        ChannelService& m_service;
        std::vector<UserListResponse::User> m_users;
        std::string m_selectedUser;
        unsigned int m_page = 1;

        std::size_t m_userButtonCount{0};
        std::unordered_map<Gx::Control*, std::size_t> m_userButtonIndices;
    };
}
