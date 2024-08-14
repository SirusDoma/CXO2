#ifndef O2JAM_PLAYING_PLAY_MENU_HPP
#define O2JAM_PLAYING_PLAY_MENU_HPP

#include <OTwo/Core/ScoreTracker.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/UI.hpp>

class ChartRenderer;
class PlayMenu : public virtual Gx::UiContainer
{
public:
    PlayMenu() = default;

    void Initialize() override;

    ChartMetadataView GetMetadata() const;
    Difficulty GetDifficulty() const;
    void SetMetadata(const ChartMetadataView &metadata, const Difficulty diff);

    void SetScoreTracker(const ScoreTracker &scores);
    const ScoreTracker* GetScoreTracker() const;

private:
    void Update(const double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;

    ChartMetadataView m_metadata;
    Difficulty m_difficulty;
    const ScoreTracker* m_scoreTracker;

    double m_elapsed = 0;
    ChartRenderer* m_renderer;
    Gx::Label* m_title;
    Gx::Animation* m_playIcon;
    Gx::Image* m_level;
    Gx::Number* m_minutes;
    Gx::Number* m_seconds;
    Gx::Gauge* m_wave;
    Gx::Gauge* m_bgmVol;
    Gx::Gauge* m_sfxVol;
    std::unordered_map<Accuracy, Gx::Number*> m_counters;
};


#endif
