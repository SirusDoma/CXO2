#include <CXO2/UI/Room/UserList.hpp>
#include <CXO2/StringTable/Identifiers/Room.hpp>
#include <CXO2/Utilities/StringFormatter.hpp>

#include <Genode/UI/List.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/Label.hpp>

#include <fmt/format.h>
#include <cmath>
#include <Genode/UI/Image.hpp>
#include <CXO2/Services/ChannelService.hpp>
#include <CXO2/States/State.hpp>

namespace Cx
{
    using namespace StringTable::Identifiers;

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

        const auto list = FindChild<Gx::List>(Resource::Room::UserList::IDC_LIST_USER_BUTTON);
        const auto listChildren = list->GetChildren();
        for (std::size_t i = 0; i < listChildren.size(); i++)
        {
            const auto userButton    = dynamic_cast<Gx::RadioButton*>(listChildren[i]);
            auto userNickLabel = userButton->FindChild<Gx::Label>(Resource::Room::UserList::IDC_TEXT_USER_NAME);
            userButton->SetCheckStateChangeCallback([=] (auto& sender)
            {
                const size_t index = ((m_page - 1) * listChildren.size()) + i;
                if (index < m_users.size() && sender.IsChecked())
                    m_selectedUser = m_users[index].Name;
            });

            for (const auto child : userButton->GetChildren())
            {
                if (const auto subControl = dynamic_cast<Gx::Image*>(child))
                    subControl->SetVisible(false);
            }

            userButton->SetFocusChangedCallback([=] (auto& sender, auto& ev)
            {
                for (auto child : sender.GetChildren())
                {
                    if (const auto subControl = dynamic_cast<Gx::Image*>(child))
                        subControl->SetVisible(sender.IsFocused());
                }
            });

            userButton->SetVisible(false);
            userButton->SetEnabled(false);
        }

        auto userCountLabel     = FindChild<Gx::Label>(Resource::Room::UserList::IDC_TEXT_USER_COUNT);
        auto btnUserRefresh     = FindChild<Gx::Button>(Resource::Room::UserList::IDC_BUTTON_REFRESH);
        const auto btnUserLeft  = FindChild<Gx::Button>(Resource::Room::UserList::IDC_BUTTON_USER_LEFT);
        const auto btnUserRight = FindChild<Gx::Button>(Resource::Room::UserList::IDC_BUTTON_USER_RIGHT);

        btnUserRefresh->SetClickCallback([=] (auto&, auto&)
        {
            OnRefreshButtonClicked();
        });

        btnUserLeft->SetClickCallback([=] (auto&, auto&)
        {
            m_page--;
            Invalidate();
        });

        btnUserRight->SetClickCallback([=] (auto&, auto&)
        {
            m_page++;
            Invalidate();
        });
    }

    void UserList::AddUser(const CharacterInfo& user)
    {
        m_users.push_back(user);
    }

    void UserList::Clear()
    {
        m_users.clear();
    }

    void UserList::OnRefreshButtonClicked()
    {
        if (m_refreshing)
            return;

        m_refreshing = true;
        m_service.GetUserList([this] (const auto& ev)
        {
            try
            {
                const auto& response = ev.Open();

                Clear();
                for (auto& user : response.Users.GetContainer())
                {
                    AddUser(CharacterInfo
                    {
                        user.Name,
                        Gender::Any,
                        Role::Normal,
                        user.Level
                    });
                }

                Invalidate();
                m_refreshing = false;
            }
            catch (...)
            {
                m_refreshing = false;
            }
        });
    }

    void UserList::Invalidate()
    {
        const auto list = FindChild<Gx::List>(Resource::Room::UserList::IDC_LIST_USER_BUTTON);
        if (!list)
            return;

        unsigned int max = static_cast<unsigned int>(std::ceil(static_cast<float>(m_users.size()) / list->GetChildrenCount()));
        max    = std::max(max, static_cast<unsigned int>(1));
        m_page = std::min(m_page, max);
        m_page = std::max(m_page, static_cast<unsigned int>(1));

        if (const auto userCountLabel = FindChild<Gx::Label>(Resource::Room::UserList::IDC_TEXT_USER_COUNT))
            userCountLabel->SetString(fmt::format("Users: {} ({}/{})", m_users.size(), m_page, max));

        const auto children = list->GetChildren();
        for (size_t i = 0; i < children.size(); i++)
        {
            const auto userButton = dynamic_cast<Gx::RadioButton*>(children[i]);
            if (!userButton)
                continue;

            const size_t index = ((m_page - 1) * list->GetChildrenCount()) + i;
            if (index < m_users.size())
            {
                const auto userNickLabel = userButton->FindChild<Gx::Label>(Resource::Room::UserList::IDC_TEXT_USER_NAME);
                if (!userNickLabel)
                    continue;

                auto user = m_users[index];
                userNickLabel->SetString(fmt::format(L"Lv.{}: {}", user.Level, user.Name));

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
