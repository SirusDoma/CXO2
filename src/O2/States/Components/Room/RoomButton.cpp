#include <O2/States/Components/Room/RoomButton.hpp>

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/Image.hpp>

RoomButton::RoomButton(Gx::Scene &scene) :
    m_scene(&scene),
    m_data()
{
    m_button   = scene.Create<Gx::Button>("Metadata/State/Room/Btn_Room/Button.json");
    auto hover = scene.Create<Gx::Image>("Metadata/State/Room/Btn_Room/Hover.json");
    m_button->SetFocusChangedCallback([hover] (auto& sender, auto& ev) { hover->SetVisible(sender.IsFocused()); });
    hover->SetVisible(false);

    m_titleLabel    = scene.Create<Gx::Label>("Metadata/State/Room/Btn_Room/Title.json");
    m_musicLabel    = scene.Create<Gx::Label>("Metadata/State/Room/Btn_Room/Music.json");
    m_capacityLabel = scene.Create<Gx::Label>("Metadata/State/Room/Btn_Room/Capacity.json");
    m_numberLabel   = scene.Create<Gx::Number>("Metadata/State/Room/Btn_Room/RoomNumber.json");
    m_speedLabel    = scene.Create<Gx::Image>("Metadata/State/Room/Btn_Room/GameSpeed.json");
    m_stateLabel    = scene.Create<Gx::Image>("Metadata/State/Room/Btn_Room/State.json");
    m_gameMode      = scene.Create<Gx::Image>("Metadata/State/Room/Btn_Room/GameMode.json");
    m_ohmLevel      = scene.Create<Gx::Image>("Metadata/State/Room/Btn_Room/OhmLevel.json");
    m_lock          = scene.Create<Gx::Image>("Metadata/State/Room/Btn_Room/Lock.json");

    AddChild(m_button, m_titleLabel, m_musicLabel, m_capacityLabel, m_numberLabel, m_speedLabel, m_stateLabel, m_gameMode, m_ohmLevel, m_lock, hover);
}

const sf::FloatRect RoomButton::GetLocalBounds() const
{
    return m_button->GetLocalBounds();
}

const RoomData RoomButton::GetRoomData() const
{
    return m_data;
}

void RoomButton::SetRoomData(const RoomData &data)
{
    m_data = data;
    Invalidate();
}

void RoomButton::Invalidate()
{
    m_titleLabel->SetString(m_data.Title);
    m_musicLabel->SetString("Lv." + std::to_string(m_data.Chart.Level) + " - " + m_data.Chart.Title);

    if (m_data.Number % 2 != 0)
        m_capacityLabel->SetString("(" + std::to_string(m_data.PlayerCount) + "/" + std::to_string(m_data.Capacity) + ")");
    else
        m_capacityLabel->SetString("(1/8)");

    m_numberLabel->SetValue(m_data.Number);
    m_numberLabel->SetDigitCount(3);
    m_gameMode->SetFrame("VS");
    m_lock->SetVisible(m_data.Locked);

    if (m_data.SpeedType == SpeedType::HiSpeed)
    {
        std::string diffName;
        switch (m_data.Difficulty)
        {
            case Difficulty::Easy:   diffName = "EX"; m_ohmLevel->SetFrame("Beginner"); break;
            case Difficulty::Normal: diffName = "NX"; m_ohmLevel->SetFrame("Intermediate"); break;
            case Difficulty::Hard:   diffName = "HX"; m_ohmLevel->SetFrame("High"); break;
            case Difficulty::Master: diffName = "MX"; m_ohmLevel->SetFrame("Master"); break;
        }

        std::string speedFrame = diffName + std::to_string(static_cast<int>(m_data.Speed));
        if(std::fmod(m_data.Speed, 1.0))
        {
            std::string speedStr(4, '\0');
            speedStr.resize(std::snprintf(&speedStr[0], speedStr.size(), "%.1f", m_data.Speed));

            speedFrame = diffName + speedStr;
        }

        m_speedLabel->SetFrame(speedFrame);
    }
    else if (m_data.SpeedType == SpeedType::RandomSpeed)
    {
        std::string speedFrame;
        switch (m_data.Difficulty)
        {
            case Difficulty::Easy:   speedFrame = "EXR"; break;
            case Difficulty::Normal: speedFrame = "NXR"; break;
            case Difficulty::Hard:   speedFrame = "HXR"; break;
            case Difficulty::Master: speedFrame = "MXR"; break;
        }

        m_speedLabel->SetFrame(speedFrame);
    }

    if (m_data.State == RoomState::Playing)
        m_stateLabel->SetFrame("Playing");
    else
        m_stateLabel->SetFrame("Waiting");
}
