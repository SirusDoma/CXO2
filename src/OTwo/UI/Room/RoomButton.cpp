#include <OTwo/UI/Room/RoomButton.hpp>
#include <OTwo/Metadata/Chart/ChartMetadata.hpp>

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/Image.hpp>
#include <OTwo/Models/Game.hpp>

RoomButton::RoomButton() :
    m_room(),
    m_hover(nullptr),
    m_active(false)
{
}

void RoomButton::Initialize()
{
    Gx::Image::Initialize();

    //m_button = FindChild<Gx::Button>("Interface/Metadata/State/Room/Btn_Room/Button.json");
    auto number   = FindChild<Gx::Number>("IDC_NUMBER_ROOM_ID");
    auto title    = FindChild<Gx::Label>("IDC_TEXT_ROOM_NAME");
    auto music    = FindChild<Gx::Label>("IDC_TEXT_MUSIC_NAME");
    auto capacity = FindChild<Gx::Label>("IDC_TEXT_CAPACITY");
    auto speed    = FindChild<Gx::Image>("IDC_IMAGE_GAME_SPEED");
    auto state    = FindChild<Gx::Image>("IDC_IMAGE_STATE");
    auto gameMode = FindChild<Gx::Image>("IDC_IMAGE_GAME_MODE");
    auto ohmLevel = FindChild<Gx::Image>("IDC_IMAGE_OHM_LEVEL");
    auto lock     = FindChild<Gx::Image>("IDC_IMAGE_PASSWORD");

    m_hover  = FindChild<Gx::Image>("IDC_IMAGE_ROOM_HOVER");
    m_hover->SetVisible(false);
    Reset();

}

sf::FloatRect RoomButton::GetLocalBounds() const
{
    return Gx::Image::GetLocalBounds();
}

const Room &RoomButton::GetRoomInfo() const
{
    return m_room;
}

void RoomButton::SetRoomInfo(const Room &data)
{
    m_room = data;
    m_active = true;
    Invalidate();
}

void RoomButton::Reset()
{
    m_room = Room();
    m_active = false;
    Invalidate();
}

bool RoomButton::IsActive() const
{
    return m_active;
}

void RoomButton::OnMouseMove(const sf::Event::MouseMoveEvent& ev)
{
    Control::OnMouseMove(ev);
    m_hover->SetVisible(IsFocused());
}

void RoomButton::Invalidate()
{
    for (const auto child : GetChildren())
    {
        if (const auto control = dynamic_cast<Gx::Control*>(child))
            control->SetVisible(m_active);
    }

    if (m_hover)
        m_hover->SetVisible(IsFocused());

    if (!m_active)
        return;

    const auto number     = FindChild<Gx::Number>("IDC_NUMBER_ROOM_ID");
    const auto title      = FindChild<Gx::Label>("IDC_TEXT_ROOM_NAME");
    const auto capacity   = FindChild<Gx::Label>("IDC_TEXT_CAPACITY");
    const auto speed      = FindChild<Gx::Image>("IDC_IMAGE_GAME_SPEED");
    const auto state      = FindChild<Gx::Image>("IDC_IMAGE_STATE");
    const auto gameMode   = FindChild<Gx::Image>("IDC_IMAGE_GAME_MODE");
    const auto ohmLevel   = FindChild<Gx::Image>("IDC_IMAGE_OHM_LEVEL");
    const auto lock       = FindChild<Gx::Image>("IDC_IMAGE_PASSWORD");
    const auto levelLimit = FindChild<Gx::Image>("IDC_IMAGE_LEVEL_LIMIT");
    const auto levelRange = FindChild<Gx::Label>("IDC_TEXT_LEVEL_RANGE");
    const auto noMusic    = FindChild<Gx::Image>("IDC_IMAGE_NOT_HAVE");

    unsigned int memberCount = 0;
    for (const auto member : m_room.Members)
    {
        if (member.ID != 0)
            memberCount++;
    }

    title->SetString(m_room.Title);
    capacity->SetString("(" + std::to_string(memberCount) + "/" + std::to_string(m_room.Capacity) + ")");

    number->SetValue(m_room.ID);
    number->SetDigitCount(3);
    lock->SetVisible(m_room.Locked);

    switch (m_room.GameMode)
    {
        case GameMode::Single: gameMode->SetFrame("Single"); break;
        case GameMode::Versus: gameMode->SetFrame("VS");     break;
        case GameMode::Album:  gameMode->SetFrame("Album");  break;
        case GameMode::Couple: gameMode->SetFrame("Couple"); break;
        default: break;
    }

    std::string speedStr(4, '\0');
    if (m_room.Speed > 0)
    {
        if (std::fmod(m_room.Speed, 1.0f) != 0)
            speedStr.resize(std::snprintf(&speedStr[0], speedStr.size(), "%.1f", m_room.Speed));
        else
            speedStr = std::to_string(static_cast<int>(m_room.Speed));
    }
    else
        speedStr = "R";

    if (m_room.SongMode == SongMode::Normal)
    {
        std::string diffName;
        switch (m_room.Difficulty)
        {
            case Difficulty::EX: diffName = "EX"; break;
            case Difficulty::NX: diffName = "NX"; break;
            case Difficulty::HX: diffName = "HX"; break;
            case Difficulty::MX: diffName = "MX"; break;
        }

        switch (m_room.ChartMetadata.GetLevelCategory())
        {
            case LevelCategory::Level1: ohmLevel->SetFrame("Beginner"); break;
            case LevelCategory::Level2: ohmLevel->SetFrame("Intermediate"); break;
            case LevelCategory::Level3: ohmLevel->SetFrame("High"); break;
            case LevelCategory::Level4: ohmLevel->SetFrame("Master"); break;
        }

        auto music = FindChild<Gx::Label>("IDC_TEXT_MUSIC_NAME");
        const auto newIndicator = FindChild<Gx::Image>("IDC_IMAGE_NEW_MUSIC");
        newIndicator->SetVisible(m_room.ChartMetadata.New);
        if (m_room.ChartMetadata.New)
        {
            music->SetVisible(false);

            music = FindChild<Gx::Label>("IDC_TEXT_NEW_MUSIC_NAME");
            music->SetVisible(true);
        }
        else
        {
            const auto newMusic = FindChild<Gx::Label>("IDC_TEXT_NEW_MUSIC_NAME");
            newMusic->SetVisible(false);
        }

        music->SetString("Lv." + std::to_string(m_room.ChartMetadata.Level) + " - " + m_room.ChartMetadata.Title);
        speed->SetFrame(diffName + speedStr);
    }
    else if (m_room.SongMode == SongMode::Random)
    {
        const auto music = FindChild<Gx::Label>("IDC_TEXT_MUSIC_NAME");
        const auto newMusic = FindChild<Gx::Label>("IDC_TEXT_NEW_MUSIC_NAME");
        const auto newIndicator = FindChild<Gx::Image>("IDC_IMAGE_NEW_MUSIC");

        music->SetString("Random");
        speed->SetFrame("RX" + speedStr);

        music->SetVisible(true);
        newMusic->SetVisible(false);
        newIndicator->SetVisible(false);
    }

    if (m_room.MinLevelLimit > 0 && m_room.MinLevelLimit < m_room.MaxLevelLimit)
    {
        levelLimit->SetVisible(true);
        levelRange->SetVisible(true);

        levelRange->SetString("Lv." + std::to_string(m_room.MinLevelLimit) + " ~ Lv." + std::to_string(m_room.MaxLevelLimit));
    }
    else
    {
        levelLimit->SetVisible(false);
        levelRange->SetVisible(false);

        levelRange->SetString("");
    }

    if (m_room.State == RoomState::Playing)
        state->SetFrame("Playing");
    else
        state->SetFrame("Waiting");

    noMusic->SetVisible(false);
}
