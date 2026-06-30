#include <CXO2/UI/Room/RoomButton.hpp>
#include <CXO2/Metadata/Chart/O2JamChartMetadata.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Models/Game.hpp>

#include <CXO2/StringTable/Identifiers/Room.hpp>
#include <CXO2/Utilities/StringFormatter.hpp>

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/Label.hpp>

#include <fmt/format.h>

namespace Cx
{
    using namespace StringTable::Identifiers;

    RoomButton::RoomButton(const SessionContext& session) :
        m_room(),
        m_music(),
        m_musicList(session.GetInstalledMusic()),
        m_hover(nullptr),
        m_active(false)
    {
    }

    void RoomButton::Initialize()
    {
        Image::Initialize();

        m_hover = FindChild<Image>(Resource::Room::Button::IDC_IMAGE_ROOM_HOVER);
        m_hover->SetVisible(false);
        Reset();
    }

    sf::FloatRect RoomButton::GetLocalBounds() const
    {
        const auto bounds = Image::GetLocalBounds();
        if (bounds == sf::FloatRect() && m_hover)
            return m_hover->GetLocalBounds();

        return bounds;
    }

    const RoomInfo& RoomButton::GetRoomInfo() const
    {
        return m_room;
    }

    void RoomButton::SetRoomInfo(const RoomInfo& data)
    {
        m_room   = data;
        m_music  = ChartMetadata{};

        if (data.MusicID > std::numeric_limits<std::uint16_t>::max())
        {
            const std::uint8_t randomBit = static_cast<std::uint8_t>((data.MusicID >> 28) & 0xFF);
            constexpr int max = static_cast<int>(LevelCategory::Level1) |
                                static_cast<int>(LevelCategory::Level2) |
                                static_cast<int>(LevelCategory::Level3) |
                                static_cast<int>(LevelCategory::Level4);

            m_randomStart = 0;
            m_randomEnd   = 0;
            if (randomBit >= 1 && randomBit <= max)
            {
                const auto levels = static_cast<int>(randomBit);
                if (levels & static_cast<int>(LevelCategory::Level1))
                {
                    if (m_randomStart == 0 || m_randomStart > 1)
                        m_randomStart = 1;

                    if (m_randomEnd < 5)
                        m_randomEnd = 5;
                }

                if (levels & static_cast<int>(LevelCategory::Level2))
                {
                    if (m_randomStart == 0 || m_randomStart > 5)
                        m_randomStart = 5;

                    if (m_randomEnd < 9)
                        m_randomEnd = 9;
                }

                if (levels & static_cast<int>(LevelCategory::Level3))
                {
                    if (m_randomStart == 0 || m_randomStart > 9)
                        m_randomStart = 9;

                    if (m_randomEnd < 13)
                        m_randomEnd = 13;
                }

                if (levels & static_cast<int>(LevelCategory::Level4))
                    m_randomEnd = 0;

                m_active = true;
            }
        }
        else if (data.MusicID > 0)
        {
            const auto it = std::find_if(m_musicList.begin(), m_musicList.end(), [id = data.MusicID] (const auto& m)
            {
               return m.ID == id;
            });

            if (it != m_musicList.end())
                m_music = *it;

            m_randomStart = 0;
            m_randomEnd = 0;
            m_active = m_music.ID != 0;
        }

        Invalidate();
    }

    void RoomButton::Reset()
    {
        m_room   = RoomInfo();
        m_music  = ChartMetadata{};
        m_active = false;

        Invalidate();
    }

    bool RoomButton::IsActive() const
    {
        return m_active;
    }

    void RoomButton::OnMouseMoved(const sf::Event::MouseMoved& ev)
    {
        Control::OnMouseMoved(ev);
        m_hover->SetVisible(IsFocused());
    }

    void RoomButton::Invalidate()
    {
        for (const auto child : GetChildren())
        {
            if (const auto control = dynamic_cast<Control*>(child))
                control->SetVisible(m_active);
        }

        if (m_hover)
            m_hover->SetVisible(IsFocused());

        if (!m_active)
            return;

        const auto number         = FindChild<Gx::BitmapNumber>(Resource::Room::Button::IDC_NUMBER_ROOM_ID);
        const auto title          = FindChild<Gx::Label>(Resource::Room::Button::IDC_TEXT_ROOM_NAME);
        const auto capacity       = FindChild<Gx::Label>(Resource::Room::Button::IDC_TEXT_CAPACITY);
        const auto speedIndicator = FindChild<Image>(Resource::Room::Button::IDC_IMAGE_GAME_SPEED);
        const auto stateIndicator = FindChild<Image>(Resource::Room::Button::IDC_IMAGE_STATE);
        const auto gameMode       = FindChild<Image>(Resource::Room::Button::IDC_IMAGE_GAME_MODE);
        const auto ohmLevel       = FindChild<Image>(Resource::Room::Button::IDC_IMAGE_OHM_LEVEL);
        const auto lock           = FindChild<Image>(Resource::Room::Button::IDC_IMAGE_PASSWORD);
        const auto levelLimit     = FindChild<Image>(Resource::Room::Button::IDC_IMAGE_LEVEL_LIMIT);
        const auto levelRange     = FindChild<Gx::Label>(Resource::Room::Button::IDC_TEXT_LEVEL_RANGE);
        const auto noMusic        = FindChild<Image>(Resource::Room::Button::IDC_IMAGE_NOT_HAVE);

        title->SetString(m_room.Title);
        capacity->SetString(fmt::format("({}/{})", m_room.UserCount, m_room.Capacity));

        number->SetValue(m_room.ID);
        number->SetDigitCount(3);
        lock->SetVisible(m_room.Locked);

        switch (m_room.Mode)
        {
            case GameMode::Single: gameMode->SetFrame("Single"); break;
            case GameMode::Versus: gameMode->SetFrame("VS");     break;
            case GameMode::Album:  gameMode->SetFrame("Album");  break;
            case GameMode::Couple: gameMode->SetFrame("Couple"); break;
            default: break;
        }

        const float speed = m_room.SpeedID < OfficialHiSpeeds.size() ? OfficialHiSpeeds[m_room.SpeedID] : -1.f;
        std::string speedStr(4, '\0');

        if (speed > 0)
        {
            if (std::fmod(speed, 1.0f) != 0)
                speedStr.resize(std::snprintf(&speedStr[0], speedStr.size(), "%.1f", speed));
            else
                speedStr = std::to_string(static_cast<int>(speed));
        }
        else
            speedStr = "R";

        if (m_room.MusicID <= std::numeric_limits<std::uint16_t>::max())
        {
            std::string diffName;
            switch (m_room.Difficulty)
            {
                case Difficulty::EX: diffName = "EX"; break;
                case Difficulty::NX: diffName = "NX"; break;
                case Difficulty::HX: diffName = "HX"; break;
                case Difficulty::MX: diffName = "MX"; break;
            }

            if (const auto it = m_music.Levels.find(m_room.Difficulty); it != m_music.Levels.end())
            {
                if (it->second <= 5)
                    ohmLevel->SetFrame("Beginner");
                else if (it->second <= 9)
                    ohmLevel->SetFrame("Intermediate");
                else if (it->second <= 13)
                    ohmLevel->SetFrame("High");
                else
                    ohmLevel->SetFrame("Master");
            }

            auto music = FindChild<Gx::Label>(Resource::Room::Button::IDC_TEXT_MUSIC_NAME);
            const auto newIndicator = FindChild<Image>(Resource::Room::Button::IDC_IMAGE_NEW_MUSIC);
            newIndicator->SetVisible(m_music.New);

            if (m_music.New)
            {
                music->SetVisible(false);

                music = FindChild<Gx::Label>(Resource::Room::Button::IDC_TEXT_NEW_MUSIC_NAME);
                music->SetVisible(true);
            }
            else
            {
                const auto newMusic = FindChild<Gx::Label>(Resource::Room::Button::IDC_TEXT_NEW_MUSIC_NAME);
                newMusic->SetVisible(false);
            }

            music->SetString(m_music.Title);
            music->SetOutlineColor(sf::Color::Black);
            speedIndicator->SetFrame(diffName + speedStr);
        }
        else
        {
            const auto music = FindChild<Gx::Label>(Resource::Room::Button::IDC_TEXT_MUSIC_NAME);
            const auto newMusic = FindChild<Gx::Label>(Resource::Room::Button::IDC_TEXT_NEW_MUSIC_NAME);
            const auto newIndicator = FindChild<Image>(Resource::Room::Button::IDC_IMAGE_NEW_MUSIC);

            if (m_randomEnd == 0 || m_randomEnd > 13)
                ohmLevel->SetFrame("Master");
            else if (m_randomEnd <= 5)
                ohmLevel->SetFrame("Beginner");
            else if (m_randomEnd <=  9)
                ohmLevel->SetFrame("Intermediate");
            else if (m_randomEnd <= 13)
                ohmLevel->SetFrame("High");

            music->SetString(fmt::format(
                "<< Random {} {} >>",
                m_randomStart == 0 ? 13 : m_randomStart,
                m_randomEnd == 0 ? "or higher" : fmt::format("- {}", m_randomEnd)
            ));
            music->SetOutlineColor(sf::Color(160, 24, 24));

            speedIndicator->SetFrame("RX" + speedStr);

            music->SetVisible(true);
            newMusic->SetVisible(false);
            newIndicator->SetVisible(false);
        }

        if (m_room.MinLevelLimit > 0 && m_room.MinLevelLimit < m_room.MaxLevelLimit)
        {
            levelLimit->SetVisible(true);
            levelRange->SetVisible(true);

            levelRange->SetString(fmt::format("Lv.{} ~ Lv.{}", m_room.MinLevelLimit, m_room.MaxLevelLimit));
        }
        else
        {
            levelLimit->SetVisible(false);
            levelRange->SetVisible(false);

            levelRange->SetString("");
        }

        if (m_room.State == RoomState::Playing)
            stateIndicator->SetFrame("Playing");
        else
            stateIndicator->SetFrame("Waiting");

        noMusic->SetVisible(false);
    }
}
