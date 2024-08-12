#include <OTwo/UI/Playing/PlayMenu.hpp>
#include <OTwo/States/State.hpp>

#include <OTwo/Core/ChartRenderer.hpp>

void PlayMenu::Initialize()
{
    const auto parent = GetParent<State>();
    if (!parent)
        return;

    const auto menu = parent->FindResource<Gx::Image>("IDC_IMAGE_PLAYING_MENU");
    AddChild(menu);

    m_title    = menu->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TITLE");
    m_playIcon = menu->FindChild<Gx::Animation>("IDC_ANIMATION_PLAY_ICON");
    m_level    = menu->FindChild<Gx::Image>("IDC_IMAGE_MUSIC_LEVEL");
    m_bgmVol   = menu->FindChild<Gx::Gauge>( "IDC_GAUGE_VOLUME_MUSIC");
    m_sfxVol   = menu->FindChild<Gx::Gauge>("IDC_GAUGE_VOLUME_EFFECT");

    if (const auto pointList = menu->FindChild<Gx::List>("IDC_LIST_NOTE_POINT_NUMBER"); pointList)
    {
        const auto children = pointList->GetChildren();
        for (int i = 0; i < children.size(); i++)
        {
            auto acc = static_cast<Accuracy>(i);
            m_counters[acc] = dynamic_cast<Gx::Number*>(children[i]);
        }
    }
}

ChartMetadataView PlayMenu::GetMetadata() const
{
    return m_metadata;
}

Difficulty PlayMenu::GetDifficulty() const
{
    return m_difficulty;
}

void PlayMenu::SetMetadata(const ChartMetadataView &metadata, const Difficulty diff)
{
    m_metadata   = metadata;
    m_difficulty = diff;

    if (m_title)
        m_title->SetString(metadata.Title);

    if (m_level)
    {
        const auto renderer = GetParent<ChartRenderer>();
        const auto speed = renderer->GetRenderSettings().Speed;
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

        std::string diffName;
        switch (m_difficulty)
        {
            case Difficulty::EX: diffName = "EX"; break;
            case Difficulty::NX: diffName = "NX"; break;
            case Difficulty::HX: diffName = "HX"; break;
            case Difficulty::MX: diffName = "MX"; break;
        }

        m_level->SetFrame(diffName + speedStr);
    }
}

void PlayMenu::SetScoreTracker(const ScoreTracker &scores)
{
    m_scoreTracker = &scores;
}

const ScoreTracker* PlayMenu::GetScoreTracker() const
{
    return m_scoreTracker;
}

void PlayMenu::Update(const double delta)
{
    if (const auto renderer = GetParent<ChartRenderer>())
    {
        if (m_playIcon)
            m_playIcon->SetDuration(sf::milliseconds((60000.f / renderer->GetCurrentBPM()) * m_playIcon->GetFrameCount()) / 4.f);

        if (m_bgmVol)
            m_bgmVol->SetValue(renderer->GetRenderSettings().Config->MusicVolume);

        if (m_sfxVol)
            m_sfxVol->SetValue(renderer->GetRenderSettings().Config->EffectVolume);
    }

    if (m_scoreTracker)
    {
        // Update score counter
        for (auto [acc, number] : m_counters)
            number->SetValue(acc == Accuracy::None ? m_scoreTracker->GetMaxCombo() : m_scoreTracker->GetPoint(acc));
    }

    UpdatableContainer::Update(delta);
}

Gx::RenderStates PlayMenu::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    return RenderableContainer::Render(surface, states);
}
