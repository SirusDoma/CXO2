#include <CXO2/UI/Components/Room/UserList.hpp>
#include <CXO2/Constants/Identifiers/Room.hpp>
#include <CXO2/Constants/Messages/Room.hpp>
#include <CXO2/Utilities/StringFormatter.hpp>

#include <CXO2/UI/List.hpp>
#include <CXO2/UI/Button.hpp>
#include <CXO2/UI/RadioButton.hpp>
#include <CXO2/UI/Label.hpp>

#include <fmt/format.h>
#include <cmath>
#include <CXO2/UI/Image.hpp>
#include <CXO2/Services/ChannelService.hpp>
#include <CXO2/States/State.hpp>

namespace Cx
{
    using namespace Constants::Identifiers;

    UserList::UserList(ChannelService& service) :
        m_service(service),
        m_users(),
        m_selectedUser(),
        m_page(1)
    {
    }

    void UserList::Initialize()
    {
        UiContainer::Initialize();

        const auto list = FindChild<List>(Resource::Room::UserList::IDC_LIST_USER_BUTTON);
        const auto listChildren = list->GetChildren();
        m_userButtonCount = listChildren.size();
        for (std::size_t i = 0; i < listChildren.size(); i++)
        {
            const auto userButton    = dynamic_cast<Cx::RadioButton*>(listChildren[i]);
            auto userNickLabel = userButton->FindChild<Label>(Resource::Room::UserList::IDC_TEXT_USER_NAME);
            m_userButtonIndices[userButton] = i;
            userButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev)
            {
                OnUserButtonCheckChanged(sender, ev);
            });

            for (const auto child : userButton->GetChildren())
            {
                if (const auto subControl = dynamic_cast<Image*>(child))
                    subControl->SetVisible(false);
            }

            userButton->SetFocusChangedCallback([this] (auto& sender, auto& ev)
            {
                OnUserButtonFocusChanged(sender, ev);
            });

            userButton->SetVisible(false);
            userButton->SetEnabled(false);
        }

        auto userCountLabel     = FindChild<Label>(Resource::Room::UserList::IDC_TEXT_USER_COUNT);
        auto btnUserRefresh     = FindChild<Cx::Button>(Resource::Room::UserList::IDC_BUTTON_REFRESH);
        const auto btnUserLeft  = FindChild<Cx::Button>(Resource::Room::UserList::IDC_BUTTON_USER_LEFT);
        const auto btnUserRight = FindChild<Cx::Button>(Resource::Room::UserList::IDC_BUTTON_USER_RIGHT);

        btnUserRefresh->SetClickCallback([this] (auto& sender, auto& ev)
        {
            OnRefreshButtonClicked(sender, ev);
        });

        btnUserLeft->SetClickCallback([this] (auto& sender, auto& ev)
        {
            OnUserLeftButtonClicked(sender, ev);
        });

        btnUserRight->SetClickCallback([this] (auto& sender, auto& ev)
        {
            OnUserRightButtonClicked(sender, ev);
        });
    }

    void UserList::AddUser(const UserListResponse::User& user)
    {
        m_users.push_back(user);
    }

    void UserList::Clear()
    {
        m_users.clear();
    }

    void UserList::OnUserListLoad(const MessageEnvelope<UserListResponse>& ev)
    {
        try
        {
            const auto& response = ev.Open();

            Clear();
            for (const auto& user : response.Users.GetContainer())
                AddUser(user);

            Invalidate();
            m_refreshing = false;
        }
        catch (...)
        {
            m_refreshing = false;
        }
    }

    void UserList::OnUserButtonCheckChanged(Cx::RadioButton& sender, Control::Event& ev)
    {
        const size_t index = ((m_page - 1) * m_userButtonCount) + m_userButtonIndices.at(&sender);
        if (index < m_users.size() && sender.IsChecked())
            m_selectedUser = m_users[index].Name.toAnsiString();
    }

    void UserList::OnUserButtonFocusChanged(Control& sender, Control::Event& ev)
    {
        for (auto child : sender.GetChildren())
        {
            if (const auto subControl = dynamic_cast<Image*>(child))
                subControl->SetVisible(sender.IsFocused());
        }
    }

    void UserList::OnRefreshButtonClicked(Control& sender, Control::Event& ev)
    {
        if (m_refreshing)
            return;

        m_refreshing = true;
        m_service.GetUserList([this] (const auto& envelope)
        {
            OnUserListLoad(envelope);
        });
    }

    void UserList::OnUserLeftButtonClicked(Control& sender, Control::Event& ev)
    {
        m_page--;
        Invalidate();
    }

    void UserList::OnUserRightButtonClicked(Control& sender, Control::Event& ev)
    {
        m_page++;
        Invalidate();
    }

    void UserList::Invalidate()
    {
        const auto list = FindChild<List>(Resource::Room::UserList::IDC_LIST_USER_BUTTON);
        if (!list)
            return;

        unsigned int max = static_cast<unsigned int>(std::ceil(static_cast<float>(m_users.size()) / list->GetChildrenCount()));
        max    = std::max(max, static_cast<unsigned int>(1));
        m_page = std::min(m_page, max);
        m_page = std::max(m_page, static_cast<unsigned int>(1));

        if (const auto userCountLabel = FindChild<Label>(Resource::Room::UserList::IDC_TEXT_USER_COUNT))
            userCountLabel->SetString(fmt::format(Constants::Messages::Room::USER_LIST_COUNT, m_users.size(), m_page, max));

        const auto children = list->GetChildren();
        for (size_t i = 0; i < children.size(); i++)
        {
            const auto userButton = dynamic_cast<Cx::RadioButton*>(children[i]);
            if (!userButton)
                continue;

            const size_t index = ((m_page - 1) * list->GetChildrenCount()) + i;
            if (index < m_users.size())
            {
                const auto userNickLabel = userButton->FindChild<Label>(Resource::Room::UserList::IDC_TEXT_USER_NAME);
                if (!userNickLabel)
                    continue;

                auto user = m_users[index];
                userNickLabel->SetString(fmt::format(Constants::Messages::Room::USER_LIST_ENTRY, user.Level, user.Name));

                userButton->SetCheckedState(user.Name == m_selectedUser);
                userButton->SetEnabled(true);
                userButton->SetVisible(true);
            }
            else
            {
                userButton->SetEnabled(false);
                userButton->SetVisible(false);
            }
        }
    }
}
