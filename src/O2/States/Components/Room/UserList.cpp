#include <O2/States/Components/Room/UserList.hpp>

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>

UserList::UserList() :
    m_list(),
    m_users(),
    m_userCountLabel(),
    m_selectedUser(),
    m_page(1)
{
}

void UserList::Initialize(Gx::Scene &scene)
{
    m_list = scene.Create<Gx::List>("Interface/Metadata/State/Room/UserList/UserButtonList.json");
    for (auto i = 0; i < USER_PER_PAGE; i++)
    {
        auto userButton    = scene.Create<Gx::RadioButton>("Interface/Metadata/State/Room/UserList/Btn_UserList.json");
        auto userNickLabel = scene.Create<Gx::Label>("Interface/Metadata/State/Room/UserList/NicknameLabel.json");
        userButton->SetCheckStateChangeCallback([=] (auto sender)
        {
            size_t index = ((m_page - 1) * USER_PER_PAGE) + i;
            if (index < m_users.size() && sender->IsChecked())
                m_selectedUser = m_users[index].PlayerID;
        });

        userButton->SetVisible(false);
        userButton->SetEnabled(false);
        userButton->AddChild(userNickLabel);

        m_list->AddChild(userButton);
    }

    m_userCountLabel    = scene.Create<Gx::Label>("Interface/Metadata/State/Room/UserList/UserCountLabel.json");
    auto btnUserRefresh = scene.Create<Gx::Button>("Interface/Metadata/State/Room/UserList/Btn_UserRefresh.json");
    auto btnUserLeft    = scene.Create<Gx::Button>("Interface/Metadata/State/Room/UserList/Btn_UserLeft.json");
    auto btnUserRight   = scene.Create<Gx::Button>("Interface/Metadata/State/Room/UserList/Btn_UserRight.json");

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

    AddChild(m_list, m_userCountLabel, btnUserRefresh, btnUserLeft, btnUserRight);
}

void UserList::AddPlayer(Room::PlayerInfo player)
{
    m_users.push_back(player);
    Invalidate();
}

void UserList::Clear()
{
    m_users.clear();
    Invalidate();
}

void UserList::Invalidate()
{
    unsigned int max = static_cast<unsigned int>(std::ceil(static_cast<float>(m_users.size()) / USER_PER_PAGE));
    max    = std::max(max, static_cast<unsigned int>(1));
    m_page = std::min(m_page, max);
    m_page = std::max(m_page, static_cast<unsigned int>(1));

    if (m_userCountLabel)
        m_userCountLabel->SetString("Users: " + std::to_string(m_users.size()) + " (" + std::to_string(m_page) + "/" + std::to_string(max) + ")");

    if (m_list)
    {
        auto children = m_list->GetChildren();
        for (size_t i = 0; i < children.size(); i++)
        {
            auto userButton = dynamic_cast<Gx::RadioButton *>(children[i]);
            if (!userButton)
                continue;

            size_t index = ((m_page - 1) * USER_PER_PAGE) + i;
            if (index < m_users.size())
            {
                auto userNickLabel = dynamic_cast<Gx::Label *>(userButton->GetChildren()[0]);
                if (!userNickLabel)
                    continue;

                auto user = m_users[index];
                userNickLabel->SetString("Lv." + std::to_string(user.Level) + ": " + user.Name);

                userButton->SetCheckedState(user.PlayerID == m_selectedUser);
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
