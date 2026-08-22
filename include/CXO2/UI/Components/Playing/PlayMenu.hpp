#pragma once

#include <CXO2/Config/GameConfig.hpp>
#include <CXO2/Core/ScoreTracker.hpp>

#include <Genode/Graphics/Animation.hpp>
#include <CXO2/UI/Image.hpp>

namespace Cx
{
    class Gauge;
    class BitmapNumber;
    class Label;

    class ChartRenderer;
    class PlayMenu : public Image
    {
    public:
        explicit PlayMenu(const GameConfig& config);

        void Initialize() override;

        ChartMetadata GetMetadata() const;
        Difficulty GetDifficulty() const;
        void SetMetadata(const ChartMetadata& metadata, Difficulty diff, float speed, SpeedMode speedMode);

        void SetScoreTracker(const ScoreTracker& scores);
        const ScoreTracker* GetScoreTracker() const;

    private:
        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        const GameConfig& m_config;

        ChartMetadata m_metadata;
        Difficulty m_difficulty;
        const ScoreTracker* m_scoreTracker;

        double m_elapsed = 0;
        ChartRenderer* m_renderer;
        Label* m_title;
        Gx::Animation* m_playIcon;
        Image* m_level;
        BitmapNumber* m_minutes;
        BitmapNumber* m_seconds;
        Gauge* m_wave;
        Gauge* m_bgmVol;
        Gauge* m_sfxVol;
        std::unordered_map<Accuracy, BitmapNumber*> m_counters;
    };
}
