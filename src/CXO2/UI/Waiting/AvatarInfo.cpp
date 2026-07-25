#include <CXO2/UI/Waiting/AvatarInfo.hpp>
#include <CXO2/Avatar/Avatar.hpp>
#include <CXO2/Contexts/RoomContext.hpp>
#include <CXO2/Constants/Identifiers/Avatar.hpp>
#include <CXO2/Utilities/StringFormatter.hpp>

#include <Genode/UI/Label.hpp>
#include <Genode/UI/Gauge.hpp>
#include <Genode/UI/Image.hpp>

#include <fmt/format.h>

namespace Cx
{
    using namespace Constants::Identifiers;

    void AvatarInfo::Initialize()
    {
        Node::Initialize();
        Invalidate();
    }

    Avatar* AvatarInfo::GetAvatar() const
    {
        return GetParent<Avatar>();
    }

    Gx::Gauge* AvatarInfo::GetLifeBar() const
    {
        return FindChild<Gx::Gauge>(Resource::Avatar::Info::IDC_GAUGE_AVATAR_INFO_LIFE);
    }

    const Room::Slot* AvatarInfo::GetSlot() const
    {
        return m_slot;
    }

    const sf::Color& AvatarInfo::GetTeamColor(const Room::Team team)
    {
        return m_teamColors[team];
    }

    void AvatarInfo::SetSlot(const Room::Slot& slot)
    {
        m_slot = &slot;
        Invalidate();
    }

    void AvatarInfo::RegisterTeamColor(const Room::Team team, const sf::Color& color)
    {
        m_teamColors[team] = color;
    }

    void AvatarInfo::Reset()
    {
        m_slot = nullptr;
        Invalidate();
    }

    sf::Color AvatarInfo::ResolveTeamColor() const
    {
        if (m_teamColors.empty())
            return m_slot->TeamColor;

        if (const auto it = m_teamColors.find(m_slot->Team); it != m_teamColors.end())
            return it->second;

        return m_slot->TeamColor;
    }

    void AvatarInfo::Invalidate()
    {
        UiContainer::Invalidate();

        if (const auto plate = FindChild<Gx::Colorable>(Resource::Avatar::Info::IDC_IMAGE_AVATAR_INFO_PLATE); plate)
        {
            if (m_slot && !m_slot->Name.isEmpty())
                plate->SetColor(ResolveTeamColor());
            else
                plate->SetColor(sf::Color::Transparent);
        }

        if (const auto readyIndicator = FindChild<Gx::Image>(Resource::Avatar::Info::IDC_IMAGE_AVATAR_READY_INDICATOR); readyIndicator)
            readyIndicator->SetVisible(m_slot && m_slot->Ready);

        const auto name  = FindChild<Gx::Label>(Resource::Avatar::Info::IDC_TEXT_AVATAR_INFO_NAME);
        const auto level = FindChild<Gx::Label>(Resource::Avatar::Info::IDC_TEXT_AVATAR_INFO_LEVEL);

        if (level)
        {
            level->SetString(m_slot ? fmt::format("Lv.{}", m_slot->Level) : std::string());

            if (name)
            {
                if (m_slot)
                {
                    name->SetString(m_slot->Name);
                    name->SetColor(ResolveTeamColor());
                }
                else
                    name->SetString(std::string());
            }
        }
        else if (name)
        {
            if (m_slot)
                name->SetString(fmt::format(L"Lv:{} {}", m_slot->Level, m_slot->Name));
            else
                name->SetString(std::string());
        }
    }
}
