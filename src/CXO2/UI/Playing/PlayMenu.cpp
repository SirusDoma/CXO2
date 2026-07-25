#include <CXO2/UI/Playing/PlayMenu.hpp>
#include <CXO2/Config/GameConfig.hpp>
#include <CXO2/Core/ChartRenderer.hpp>
#include <CXO2/States/State.hpp>

#include <CXO2/Constants/Identifiers/Game.hpp>

#include <Genode/UI/List.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/Gauge.hpp>
#include <Genode/UI/Label.hpp>

#include <fmt/format.h>

namespace Cx
{
    using namespace Constants::Identifiers;

    PlayMenu::PlayMenu(const GameConfig& config) :
        m_config(config),
        m_difficulty(Difficulty::EX), m_scoreTracker(),
        m_renderer(nullptr),
        m_title(nullptr),
        m_playIcon(nullptr),
        m_level(nullptr),
        m_minutes(nullptr),
        m_seconds(nullptr),
        m_wave(nullptr),
        m_bgmVol(nullptr),
        m_sfxVol(nullptr)
    {
    }

    void PlayMenu::Initialize()
    {
        Image::Initialize();

        m_renderer = GetParent<Cx::State>()->FindChild<ChartRenderer>(Resource::Game::Menu::IDC_CHART_RENDERER);
        m_title    = FindChild<Gx::Label>(Resource::Game::Menu::IDC_TEXT_MUSIC_TITLE);
        m_playIcon = FindChild<Gx::Animation>(Resource::Game::Menu::IDC_ANIMATION_PLAY_ICON);
        m_level    = FindChild<Image>(Resource::Game::Menu::IDC_IMAGE_MUSIC_LEVEL);
        m_wave     = FindChild<Gx::Gauge>(Resource::Game::Menu::IDC_GAUGE_WAVE);
        m_minutes  = FindChild<Gx::BitmapNumber>(Resource::Game::Menu::IDC_NUMBER_PLAY_TIME_MINUTE);
        m_seconds  = FindChild<Gx::BitmapNumber>(Resource::Game::Menu::IDC_NUMBER_PLAY_TIME_SECOND);
        m_bgmVol   = FindChild<Gx::Gauge>(Resource::Game::Menu::IDC_GAUGE_VOLUME_MUSIC);
        m_sfxVol   = FindChild<Gx::Gauge>(Resource::Game::Menu::IDC_GAUGE_VOLUME_EFFECT);

        if (const auto pointList = FindChild<Gx::List>(Resource::Game::Menu::IDC_LIST_NOTE_POINT_NUMBER); pointList)
        {
            const auto children = pointList->GetChildren();
            for (std::size_t i = 0; i < children.size(); i++)
            {
                auto acc = static_cast<Accuracy>(i);
                m_counters[acc] = dynamic_cast<Gx::BitmapNumber*>(children[i]);
            }
        }
    }

    ChartMetadata PlayMenu::GetMetadata() const
    {
        return m_metadata;
    }

    Difficulty PlayMenu::GetDifficulty() const
    {
        return m_difficulty;
    }

    void PlayMenu::SetMetadata(const ChartMetadata& metadata, const Difficulty diff, const float speed, const SpeedMode speedMode)
    {
        m_metadata   = metadata;
        m_difficulty = diff;

        if (m_title)
            m_title->SetString(metadata.Title);

        if (m_wave)
        {
            m_wave->SetMaximumValue(metadata.Durations.find(diff)->second.asSeconds());
            m_wave->SetValue(0);
        }

        if (m_level)
        {
            std::string speedStr = "R";
            if (speedMode != SpeedMode::XrSpeed && speedMode != SpeedMode::TdSpeed && speed > 0)
                speedStr = fmt::format("{}", speed);

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
            m_bgmVol->SetValue(m_config.MusicVolume);

        if (m_sfxVol)
            m_sfxVol->SetValue(m_config.EffectVolume);
    }

    void PlayMenu::SetScoreTracker(const ScoreTracker& scores)
    {
        m_scoreTracker = &scores;
    }

    const ScoreTracker* PlayMenu::GetScoreTracker() const
    {
        return m_scoreTracker;
    }

    void PlayMenu::Update(const sf::Time& delta)
    {
        if (!m_renderer)
            m_renderer = GetParent<Cx::State>()->FindChild<ChartRenderer>(Resource::Game::Menu::IDC_CHART_RENDERER);

        if (m_renderer && m_renderer->IsRendering())
        {
            if (m_elapsed < m_metadata.Durations[m_difficulty].asMilliseconds())
                m_elapsed += delta.asMilliseconds();

            if (m_wave)
                m_wave->SetValue(std::floor(m_elapsed / 1000.f));

            if (m_minutes)
                m_minutes->SetValue(std::floor((m_elapsed / 1000.f) / 60.f));

            if (m_seconds)
                m_seconds->SetValue(static_cast<int>(std::floor((m_elapsed / 1000.f))) % 60);

            if (m_playIcon)
                m_playIcon->SetDuration(sf::milliseconds((60000.f / m_renderer->GetCurrentBPM()) * m_playIcon->GetFrameCount()) / 4.f);

            if (m_bgmVol)
                m_bgmVol->SetValue(m_renderer->GetRenderSettings().Config.MusicVolume);

            if (m_sfxVol)
                m_sfxVol->SetValue(m_renderer->GetRenderSettings().Config.EffectVolume);
        }

        if (m_scoreTracker)
        {
            // Update score counter
            for (auto [acc, number] : m_counters)
                number->SetValue(acc == Accuracy::None ? m_scoreTracker->GetMaxCombo() : m_scoreTracker->GetPoint(acc));
        }

        Image::Update(delta);
    }

    Gx::RenderStates PlayMenu::Render(Gx::RenderSurface& surface, const Gx::RenderStates states) const
    {
        return Image::Render(surface, states);
    }
}
