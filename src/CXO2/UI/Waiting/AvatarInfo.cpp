#include <CXO2/UI/Waiting/AvatarInfo.hpp>
#include <CXO2/Avatar/Avatar.hpp>
#include <CXO2/Contexts/RoomContext.hpp>
#include <CXO2/StringTable/Identifiers/Avatar.hpp>
#include <CXO2/Utilities/StringFormatter.hpp>

#include <Genode/UI/Label.hpp>
#include <Genode/UI/Gauge.hpp>
#include <Genode/UI/Image.hpp>

#include <fmt/format.h>

namespace Cx
{
    using namespace StringTable::Identifiers;

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

    RoomSlot* AvatarInfo::GetSlot() const
    {
        return m_slot;
    }

    const sf::Color& AvatarInfo::GetTeamColor(const RoomTeam team)
    {
        return m_teamColors[team];
    }

    void AvatarInfo::SetSlot(RoomSlot& slot)
    {
        m_slot = &slot;
        Invalidate();
    }

    void AvatarInfo::RegisterTeamColor(const RoomTeam team, const sf::Color& color)
    {
        m_teamColors[team] = color;
    }

    void AvatarInfo::Reset()
    {
        m_slot = nullptr;
        Invalidate();
    }

    sf::Color AvatarInfo::ResolveTeamColor()
    {
        if (m_teamColors.empty())
            return m_slot->TeamColor;

        const auto& color = GetTeamColor(m_slot->Team);
        m_slot->TeamColor = color;

        return color;
    }

    void AvatarInfo::Invalidate()
    {
        UiContainer::Invalidate();

        if (const auto plate = FindChild<Gx::Colorable>(Resource::Avatar::Info::IDC_IMAGE_AVATAR_INFO_PLATE); plate)
        {
            if (m_slot && !m_slot->Member->Name.isEmpty())
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
            level->SetString(m_slot ? fmt::format("Lv.{}", m_slot->Member->Level) : std::string());

            if (name)
            {
                if (m_slot)
                {
                    name->SetString(m_slot->Member->Name);
                    name->SetColor(ResolveTeamColor());
                }
                else
                    name->SetString(std::string());
            }
        }
        else if (name)
        {
            if (m_slot)
                name->SetString(fmt::format(L"Lv:{} {}", m_slot->Member->Level, m_slot->Member->Name));
            else
                name->SetString(std::string());
        }
    }
}
