#pragma once

#include <CXO2/UI/UiContainer.hpp>

#include <CXO2/Network/MessageEnvelope.hpp>
#include <CXO2/Network/Responses/UserListResponse.hpp>

#include <unordered_map>
#include <vector>

namespace Cx
{
    class ChannelService;
    class UserList : public Cx::UiContainer
    {
    public:
        explicit UserList(ChannelService& service);
        void Initialize() override;

        void AddUser(const UserListResponse::User& user);
        void Clear();

        void Invalidate() override;

    private:
        void OnUserListLoad(const MessageEnvelope<UserListResponse>& ev);

        void OnUserButtonCheckChanged(Cx::RadioButton& sender, Control::Event& ev);
        void OnUserButtonFocusChanged(Control& sender, Control::Event& ev);

        void OnRefreshButtonClicked(Control& sender, Control::Event& ev);
        void OnUserLeftButtonClicked(Control& sender, Control::Event& ev);
        void OnUserRightButtonClicked(Control& sender, Control::Event& ev);

        bool m_refreshing{false};

        ChannelService& m_service;
        std::vector<UserListResponse::User> m_users;
        std::string m_selectedUser;
        unsigned int m_page = 1;

        std::size_t m_userButtonCount{0};
        std::unordered_map<Control*, std::size_t> m_userButtonIndices;
    };
}
