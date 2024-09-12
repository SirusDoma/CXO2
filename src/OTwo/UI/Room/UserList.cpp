#include <OTwo/UI/Room/UserList.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>

#include <cmath>

UserList::UserList() :
    m_users(),
    m_selectedUser(),
    m_page(1)
{
}

void UserList::Initialize()
{
    Gx::UiContainer::Initialize();

    auto list = FindChild<Gx::List>("IDC_LIST_USER_BUTTON");
    auto listChildren = list->GetChildren();
    for (std::size_t i = 0; i < listChildren.size(); i++)
    {
        auto userButton    = dynamic_cast<Gx::RadioButton*>(listChildren[i]);
        auto userNickLabel = userButton->FindChild<Gx::Label>("IDC_TEXT_USER_NAME");
        userButton->SetCheckStateChangeCallback([=] (auto sender)
        {
            size_t index = ((m_page - 1) * listChildren.size()) + i;
            if (index < m_users.size() && sender->IsChecked())
                m_selectedUser = m_users[index].ID;
        });

        userButton->SetVisible(false);
        userButton->SetEnabled(false);
    }

    auto userCountLabel = FindChild<Gx::Label>("IDC_TEXT_USER_COUNT");
    auto btnUserRefresh = FindChild<Gx::Button>("IDC_BUTTON_REFRESH");
    auto btnUserLeft    = FindChild<Gx::Button>("IDC_BUTTON_USER_LEFT");
    auto btnUserRight   = FindChild<Gx::Button>("IDC_BUTTON_USER_RIGHT");

    btnUserLeft->SetClickCallback([=] (auto& sender, auto& ev)
    {
        m_page--;
        Invalidate();
    });

    btnUserRight->SetClickCallback([=] (auto& sender, auto& ev)
    {
        m_page++;
        Invalidate();
    });
}

void UserList::AddUser(Player user)
{
    m_users.push_back(user);
    Invalidate();
}

void UserList::Clear()
{
    m_users.clear();
    Invalidate();
}

void UserList::Invalidate()
{
    auto list = FindChild<Gx::List>("IDC_LIST_USER_BUTTON");
    if (!list)
        return;

    unsigned int max = static_cast<unsigned int>(std::ceil(static_cast<float>(m_users.size()) / list->GetChildren().size()));
    max    = std::max(max, static_cast<unsigned int>(1));
    m_page = std::min(m_page, max);
    m_page = std::max(m_page, static_cast<unsigned int>(1));

    auto userCountLabel = FindChild<Gx::Label>("IDC_TEXT_USER_COUNT");
    if (userCountLabel)
        userCountLabel->SetString("Users: " + std::to_string(m_users.size()) + " (" + std::to_string(m_page) + "/" + std::to_string(max) + ")");

    auto children = list->GetChildren();
    for (size_t i = 0; i < children.size(); i++)
    {
        auto userButton = dynamic_cast<Gx::RadioButton*>(children[i]);
        if (!userButton)
            continue;

        size_t index = ((m_page - 1) * list->GetChildren().size()) + i;
        if (index < m_users.size())
        {
            auto userNickLabel = userButton->FindChild<Gx::Label>("IDC_TEXT_USER_NAME");
            if (!userNickLabel)
                continue;

            auto user = m_users[index];
            userNickLabel->SetString("Lv." + std::to_string(user.Level) + ": " + user.Name);

            userButton->SetCheckedState(user.ID == m_selectedUser);
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
