#include <OTwo/UI/Waiting/AvatarInfo.hpp>
#include <OTwo/Avatar/Avatar.hpp>

#include <Genode/UI/Label.hpp>
#include <Genode/UI/Gauge.hpp>
#include <Genode/UI/Image.hpp>

void AvatarInfo::Initialize()
{
    Node::Initialize();
    Invalidate();
}

Avatar *AvatarInfo::GetAvatar() const
{
    return GetParent<Avatar>();
}

Gx::Gauge *AvatarInfo::GetLifeBar() const
{
    return FindChild<Gx::Gauge>("IDC_GAUGE_AVATAR_INFO_LIFE");
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
        if (m_member && m_member->ID != 0)
        {
            if (!m_teamColors.empty())
            {
                const auto& color = GetTeamColor(m_member->Team);
                plate->SetColor(color);
                m_member->Color = color;
            }
            else
                plate->SetColor(m_member->Color);
        }
        else
            plate->SetColor(sf::Color::Transparent);
    }

    if (const auto readyIndicator = FindChild<Gx::Image>("IDC_IMAGE_AVATAR_READY_INDICATOR"); readyIndicator)
        readyIndicator->SetVisible(m_member && m_member->Ready);

    if (const auto level = FindChild<Gx::Label>("IDC_TEXT_AVATAR_INFO_LEVEL"); level)
    {
        if (m_member)
            level->SetString("Lv." + std::to_string(m_member->Level));
        else
            level->SetString(std::string());

        if (const auto name = FindChild<Gx::Label>("IDC_TEXT_AVATAR_INFO_NAME"); name)
        {
            if (m_member)
            {
                name->SetString(m_member->Name);
                if (!m_teamColors.empty())
                {
                    const auto& color = GetTeamColor(m_member->Team);
                    name->SetColor(GetTeamColor(m_member->Team));
                    m_member->Color = color;
                }
                else
                    name->SetColor(m_member->Color);

            }
            else
                name->SetString(std::string());
        }
    }
    else if (const auto label = FindChild<Gx::Label>("IDC_TEXT_AVATAR_INFO_NAME"); label)
    {
        if (m_member)
            label->SetString("Lv:" + std::to_string(m_member->Level) + " " + m_member->Name);
        else
            label->SetString(std::string());
    }
}
