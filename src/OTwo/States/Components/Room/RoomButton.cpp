#include <OTwo/States/Components/Room/RoomButton.hpp>

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/Image.hpp>

RoomButton::RoomButton() :
    m_room(),
    m_hover(nullptr),
    m_active(false)
{
}

void RoomButton::Initialize()
{
    //m_button = FindChild<Gx::Button>("Interface/Metadata/State/Room/Btn_Room/Button.json");
    m_hover  = FindChild<Gx::Image>("IDC_IMAGE_ROOM_HOVER");
    m_hover->SetVisible(false);
    Reset();

    auto number   = FindChild<Gx::Number>("IDC_NUMBER_ROOM_ID");
    auto title    = FindChild<Gx::Label>("IDC_TEXT_ROOM_NAME");
    auto music    = FindChild<Gx::Label>("IDC_TEXT_MUSIC_NAME");
    auto capacity = FindChild<Gx::Label>("IDC_TEXT_CAPACITY");
    auto speed    = FindChild<Gx::Image>("IDC_IMAGE_GAME_SPEED");
    auto state    = FindChild<Gx::Image>("IDC_IMAGE_STATE");
    auto gameMode = FindChild<Gx::Image>("IDC_IMAGE_GAME_MODE");
    auto ohmLevel = FindChild<Gx::Image>("IDC_IMAGE_OHM_LEVEL");
    auto lock     = FindChild<Gx::Image>("IDC_IMAGE_PASSWORD");

}

sf::FloatRect RoomButton::GetLocalBounds() const
{
    return Gx::Image::GetLocalBounds();
}

const RoomData &RoomButton::GetRoomData() const
{
    return m_room;
}

void RoomButton::SetRoomData(const RoomData &data)
{
    m_room = data;
    m_active = true;
    Invalidate();
}

void RoomButton::Reset()
{
    m_room = RoomData();
    m_active = false;
    Invalidate();
}

bool RoomButton::IsActive() const
{
    return m_active;
}

void RoomButton::OnMouseMove(sf::Event::MouseMoveEvent ev)
{
    Control::OnMouseMove(ev);
    m_hover->SetVisible(IsFocused());
}

void RoomButton::Invalidate()
{
    for (auto child : GetChildren())
    {
        auto control = dynamic_cast<Gx::Control*>(child);
        if (control)
            control->SetVisible(m_active);
    }

    if (m_hover)
        m_hover->SetVisible(IsFocused());

    if (!m_active)
        return;

    auto number     = FindChild<Gx::Number>("IDC_NUMBER_ROOM_ID");
    auto title      = FindChild<Gx::Label>("IDC_TEXT_ROOM_NAME");
    auto capacity   = FindChild<Gx::Label>("IDC_TEXT_CAPACITY");
    auto speed      = FindChild<Gx::Image>("IDC_IMAGE_GAME_SPEED");
    auto state      = FindChild<Gx::Image>("IDC_IMAGE_STATE");
    auto gameMode   = FindChild<Gx::Image>("IDC_IMAGE_GAME_MODE");
    auto ohmLevel   = FindChild<Gx::Image>("IDC_IMAGE_OHM_LEVEL");
    auto lock       = FindChild<Gx::Image>("IDC_IMAGE_PASSWORD");
    auto levelLimit = FindChild<Gx::Image>("IDC_IMAGE_LEVEL_LIMIT");
    auto levelRange = FindChild<Gx::Label>("IDC_TEXT_LEVEL_RANGE");
    auto noMusic    = FindChild<Gx::Image>("IDC_IMAGE_NOT_HAVE");

    title->SetString(m_room.Title);
    capacity->SetString("(" + std::to_string(m_room.PlayerCount) + "/" + std::to_string(m_room.Capacity) + ")");

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
            case Difficulty::Easy:   diffName = "EX"; ohmLevel->SetFrame("Beginner"); break;
            case Difficulty::Normal: diffName = "NX"; ohmLevel->SetFrame("Intermediate"); break;
            case Difficulty::Hard:   diffName = "HX"; ohmLevel->SetFrame("High"); break;
            case Difficulty::Master: diffName = "MX"; ohmLevel->SetFrame("Master"); break;
        }

        auto music = FindChild<Gx::Label>("IDC_TEXT_MUSIC_NAME");
        auto newIndicator = FindChild<Gx::Image>("IDC_IMAGE_NEW_MUSIC");
        newIndicator->SetVisible(m_room.Chart.New);
        if (m_room.Chart.New)
        {
            music->SetVisible(false);

            music = FindChild<Gx::Label>("IDC_TEXT_NEW_MUSIC_NAME");
            music->SetVisible(true);
        }
        else
        {
            auto newMusic = FindChild<Gx::Label>("IDC_TEXT_NEW_MUSIC_NAME");
            newMusic->SetVisible(false);
        }

        music->SetString("Lv." + std::to_string(m_room.Chart.Level) + " - " + m_room.Chart.Title);
        speed->SetFrame(diffName + speedStr);
    }
    else if (m_room.SongMode == SongMode::Random)
    {
        auto music = FindChild<Gx::Label>("IDC_TEXT_MUSIC_NAME");
        auto newMusic = FindChild<Gx::Label>("IDC_TEXT_NEW_MUSIC_NAME");
        auto newIndicator = FindChild<Gx::Image>("IDC_IMAGE_NEW_MUSIC");

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
