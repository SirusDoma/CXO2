#include <OTwo/States/Components/Waiting/AvatarContainer.hpp>
#include <OTwo/Avatar/Avatar.hpp>

#include <Genode/UI.hpp>

AvatarContainer::AvatarContainer(const Gx::UiContainer &&other) noexcept :
    Gx::UiContainer(other),
    Gx::Node(other),
    m_player(nullptr)
{
}

void AvatarContainer::Initialize()
{
    Node::Initialize();
    Invalidate();
}

const Player *AvatarContainer::GetPlayer() const
{
    return m_player;
}

void AvatarContainer::SetPlayer(const Player &player)
{
    m_player = &player;
}

void AvatarContainer::Invalidate()
{
    UiContainer::Invalidate();

    if (!m_player || m_player->ID == 0)
    {
        if (auto avatar = FindChild<Avatar>("IDC_AVATAR"); avatar)
            avatar->ClearEquipments();

        if (auto nickname = FindChild<Gx::Label>("IDC_TEXT_NICKNAME"); nickname)
        {
            nickname->SetString("");
            nickname->SetVisible(false);
        }
    }
}
