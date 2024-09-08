#include <OTwo/UI/Playing/PlayMenu.hpp>
#include <OTwo/Core/ChartRenderer.hpp>

#include <OTwo/States/State.hpp>

void PlayMenu::Initialize()
{
    Gx::UiContainer::Initialize();

    const auto parent = GetParent<::State>();
    if (!parent)
        return;

    const auto menu = parent->FindResource<Gx::Image>("IDC_IMAGE_PLAYING_MENU");
    AddChild(menu);

    m_renderer = parent->FindChild<ChartRenderer>("IDC_CHART_RENDERER");
    m_title    = menu->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TITLE");
    m_playIcon = menu->FindChild<Gx::Animation>("IDC_ANIMATION_PLAY_ICON");
    m_level    = menu->FindChild<Gx::Image>("IDC_IMAGE_MUSIC_LEVEL");
    m_wave     = menu->FindChild<Gx::Gauge>("IDC_GAUGE_WAVE");
    m_minutes  = menu->FindChild<Gx::Number>("IDC_NUMBER_PLAY_TIME_MINUTE");
    m_seconds  = menu->FindChild<Gx::Number>("IDC_NUMBER_PLAY_TIME_SECOND");
    m_bgmVol   = menu->FindChild<Gx::Gauge>("IDC_GAUGE_VOLUME_MUSIC");
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

    SetBatchingEnabled(true);
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

    if (m_wave)
        m_wave->SetMaximumValue(metadata.Duration.asSeconds());

    const auto  state   = GetParent<::State>();
    const auto& context = state->Require<GameContext>();
    if (m_level)
    {
        const auto  speed = context.GetSpeed();
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

    if (m_bgmVol)
        m_bgmVol->SetValue(context.GetConfig()->MusicVolume);

    if (m_sfxVol)
        m_sfxVol->SetValue(context.GetConfig()->EffectVolume);
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
    if (m_renderer && m_renderer->IsStarted())
    {
        if (m_elapsed < m_metadata.Duration.asMilliseconds())
            m_elapsed += delta;

        if (m_wave)
            m_wave->SetValue(std::floor(m_elapsed / 1000.f));

        if (m_minutes)
            m_minutes->SetValue(std::floor((m_elapsed / 1000.f) / 60.f));

        if (m_seconds)
            m_seconds->SetValue(static_cast<int>(std::floor((m_elapsed / 1000.f))) % 60);

        if (m_playIcon)
            m_playIcon->SetDuration(sf::milliseconds((60000.f / m_renderer->GetCurrentBPM()) * m_playIcon->GetFrameCount()) / 4.f);

        if (m_bgmVol)
            m_bgmVol->SetValue(m_renderer->GetRenderSettings().Config->MusicVolume);

        if (m_sfxVol)
            m_sfxVol->SetValue(m_renderer->GetRenderSettings().Config->EffectVolume);
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
