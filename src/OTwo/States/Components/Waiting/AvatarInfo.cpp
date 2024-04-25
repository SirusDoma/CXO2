#include <OTwo/States/Components/Waiting/AvatarInfo.hpp>
#include <OTwo/Avatar/Avatar.hpp>

#include <Genode/UI/Label.hpp>

void AvatarInfo::Initialize()
{
    Node::Initialize();
    Invalidate();
}

Avatar *AvatarInfo::GetAvatar() const
{
    return GetParent<Avatar>();
}

RoomMember *AvatarInfo::GetMember() const
{
    return m_member;
}

const sf::Color &AvatarInfo::GetTeamColor(RoomTeam team)
{
    return m_teamColors[team];
}

void AvatarInfo::SetMember(RoomMember &member)
{
    m_member = &member;
    Invalidate();
}

void AvatarInfo::RegisterTeamColor(RoomTeam team, const sf::Color &color)
{
    m_teamColors[team] = color;
}

void AvatarInfo::Reset()
{
    m_member = nullptr;
    Invalidate();
}

void AvatarInfo::Invalidate()
{
    UiContainer::Invalidate();

    if (const auto plate = FindChild<Gx::Colorable>("IDC_IMAGE_AVATAR_INFO_PLATE"); plate)
    {
        if (m_member)
            plate->SetColor(GetTeamColor(m_member->Team));
        else
            plate->SetColor(sf::Color::Transparent);

        if (const auto label = FindChild<Gx::Label>("IDC_TEXT_AVATAR_INFO_NAME"); label)
        {
            if (m_member)
                label->SetString("Lv: " + std::to_string(m_member->Level) + " " + m_member->Name);
            else
                label->SetString(std::string());
        }
    }
    else
    {
        if (const auto label = FindChild<Gx::Label>("IDC_TEXT_AVATAR_INFO_NAME"); label)
        {
            if (m_member)
                label->SetString("Lv." + std::to_string(m_member->Level));
            else
                label->SetString(std::string());
        }

        if (const auto label = FindChild<Gx::Label>("IDC_TEXT_AVATAR_INFO_NAME"); label)
        {
            if (m_member)
            {
                label->SetString(m_member->Name);
                label->SetColor(GetTeamColor(m_member->Team));
            }
            else
                label->SetString(std::string());
        }
    }


}
