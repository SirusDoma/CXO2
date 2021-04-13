#include <O2/States/Components/Room/RoomButton.hpp>

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/Image.hpp>

RoomButton::RoomButton() :
    m_data(),
    m_active(false)
{
}

void RoomButton::Initialize(Gx::Scene &scene)
{
    m_button = scene.Create<Gx::Button>("Interface/Metadata/State/Room/Btn_Room/Button.json");
    m_hover  = scene.Create<Gx::Image>("Interface/Metadata/State/Room/Btn_Room/Hover.json");
    m_hover->SetVisible(false);

    m_titleLabel    = scene.Create<Gx::Label>("Interface/Metadata/State/Room/Btn_Room/TitleLabel.json");
    m_musicLabel    = scene.Create<Gx::Label>("Interface/Metadata/State/Room/Btn_Room/MusicLabel.json");
    m_capacityLabel = scene.Create<Gx::Label>("Interface/Metadata/State/Room/Btn_Room/Capacity.json");
    m_numberLabel   = scene.Create<Gx::Number>("Interface/Metadata/State/Room/Btn_Room/RoomNumber.json");
    m_speedLabel    = scene.Create<Gx::Image>("Interface/Metadata/State/Room/Btn_Room/GameSpeed.json");
    m_stateLabel    = scene.Create<Gx::Image>("Interface/Metadata/State/Room/Btn_Room/State.json");
    m_gameMode      = scene.Create<Gx::Image>("Interface/Metadata/State/Room/Btn_Room/GameMode.json");
    m_ohmLevel      = scene.Create<Gx::Image>("Interface/Metadata/State/Room/Btn_Room/OhmLevel.json");
    m_lock          = scene.Create<Gx::Image>("Interface/Metadata/State/Room/Btn_Room/Lock.json");

    AddChild(m_button, m_titleLabel, m_musicLabel, m_capacityLabel, m_numberLabel, m_speedLabel, m_stateLabel, m_gameMode, m_ohmLevel, m_lock, m_hover);
}

const sf::FloatRect RoomButton::GetLocalBounds() const
{
    return m_button->GetLocalBounds();
}

bool RoomButton::IsActive() const
{
    return m_active;
}

const Room::RoomData RoomButton::GetRoomData() const
{
    return m_data;
}

void RoomButton::SetRoomData(const Room::RoomData &data)
{
    m_data   = data;
    m_active = true;

    Invalidate();
}

void RoomButton::Deactivate()
{
    m_data   = Room::RoomData();
    m_active = false;

    Invalidate();
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

    m_button->SetVisible(true);
    m_hover->SetVisible(IsFocused());
    if (!m_active)
        return;

    m_titleLabel->SetString(m_data.Title);
    m_capacityLabel->SetString("(" + std::to_string(m_data.PlayerCount) + "/" + std::to_string(m_data.Capacity) + ")");

    m_numberLabel->SetValue(m_data.Number);
    m_numberLabel->SetDigitCount(3);
    m_lock->SetVisible(m_data.Locked);

    switch (m_data.GameMode)
    {
        case Room::GameMode::Single: m_gameMode->SetFrame("Single"); break;
        case Room::GameMode::Vs:     m_gameMode->SetFrame("VS");     break;
        case Room::GameMode::Album:  m_gameMode->SetFrame("Album");  break;
        case Room::GameMode::Couple: m_gameMode->SetFrame("Couple"); break;
    }

    std::string speedStr(4, '\0');
    if (m_data.Speed > 0)
    {
        if (std::fmod(m_data.Speed, 1.0))
            speedStr.resize(std::snprintf(&speedStr[0], speedStr.size(), "%.1f", m_data.Speed));
        else
            speedStr = std::to_string(static_cast<int>(m_data.Speed));
    }
    else
        speedStr = "R";

    if (m_data.SongMode == SongMode::User)
    {
        std::string diffName;
        switch (m_data.Difficulty)
        {
            case Difficulty::Easy:   diffName = "EX"; m_ohmLevel->SetFrame("Beginner"); break;
            case Difficulty::Normal: diffName = "NX"; m_ohmLevel->SetFrame("Intermediate"); break;
            case Difficulty::Hard:   diffName = "HX"; m_ohmLevel->SetFrame("High"); break;
            case Difficulty::Master: diffName = "MX"; m_ohmLevel->SetFrame("Master"); break;
        }

        m_musicLabel->SetString("Lv." + std::to_string(m_data.Chart.Level) + " - " + m_data.Chart.Title);
        m_speedLabel->SetFrame(diffName + speedStr);
    }
    else if (m_data.SongMode == SongMode::Random)
    {
        m_musicLabel->SetString("Random");
        m_speedLabel->SetFrame("RX" + speedStr);
    }

    if (m_data.State == Room::RoomState::Playing)
        m_stateLabel->SetFrame("Playing");
    else
        m_stateLabel->SetFrame("Waiting");
}
