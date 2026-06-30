#include <CXO2/UI/Waiting/AvatarInfo.hpp>
#include <CXO2/Avatar/Avatar.hpp>
#include <CXO2/StringTable/Identifiers/Avatar.hpp>
#include <CXO2/Utilities/StringFormatter.hpp>

#include <Genode/UI/Label.hpp>
#include <Genode/UI/Gauge.hpp>
#include <Genode/UI/Image.hpp>

#include <fmt/format.h>
#include <CXO2/Contexts/RoomContext.hpp>

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

    const sf::Color& AvatarInfo::GetTeamColor(RoomTeam team)
    {
        return m_teamColors[team];
    }

    void AvatarInfo::SetSlot(RoomSlot& slot)
    {
        m_slot = &slot;
        Invalidate();
    }

    void AvatarInfo::RegisterTeamColor(RoomTeam team, const sf::Color& color)
    {
        m_teamColors[team] = color;
    }

    void AvatarInfo::Reset()
    {
        m_slot = nullptr;
        Invalidate();
    }

    void AvatarInfo::Invalidate()
    {
        UiContainer::Invalidate();

        if (const auto plate = FindChild<Gx::Colorable>(Resource::Avatar::Info::IDC_IMAGE_AVATAR_INFO_PLATE); plate)
        {
            if (m_slot && !m_slot->Member->Name.isEmpty())
            {
                if (!m_teamColors.empty())
                {
                    const auto& color = GetTeamColor(m_slot->Team);
                    plate->SetColor(color);
                    m_slot->TeamColor = color;
                }
                else
                    plate->SetColor(m_slot->TeamColor);
            }
            else
                plate->SetColor(sf::Color::Transparent);
        }

        if (const auto readyIndicator = FindChild<Gx::Image>(Resource::Avatar::Info::IDC_IMAGE_AVATAR_READY_INDICATOR); readyIndicator)
            readyIndicator->SetVisible(m_slot && m_slot->Ready);

        if (const auto level = FindChild<Gx::Label>(Resource::Avatar::Info::IDC_TEXT_AVATAR_INFO_LEVEL); level)
        {
            if (m_slot)
                level->SetString(fmt::format("Lv.{}", m_slot->Member->Level));
            else
                level->SetString(std::string());

            if (const auto name = FindChild<Gx::Label>(Resource::Avatar::Info::IDC_TEXT_AVATAR_INFO_NAME); name)
            {
                if (m_slot)
                {
                    name->SetString(m_slot->Member->Name);
                    if (!m_teamColors.empty())
                    {
                        const auto& color = GetTeamColor(m_slot->Team);
                        name->SetColor(GetTeamColor(m_slot->Team));
                        m_slot->TeamColor = color;
                    }
                    else
                        name->SetColor(m_slot->TeamColor);

                }
                else
                    name->SetString(std::string());
            }
        }
        else if (const auto label = FindChild<Gx::Label>(Resource::Avatar::Info::IDC_TEXT_AVATAR_INFO_NAME); label)
        {
            if (m_slot)
                label->SetString(fmt::format(L"Lv:{} {}", m_slot->Member->Level, m_slot->Member->Name));
            else
                label->SetString(std::string());
        }
    }
}
