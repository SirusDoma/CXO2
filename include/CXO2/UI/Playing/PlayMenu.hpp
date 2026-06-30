#pragma once

#include <CXO2/Core/ScoreTracker.hpp>

#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/Image.hpp>

namespace Gx
{
    class Gauge;
    class BitmapNumber;
    class Label;
}

namespace Cx
{

    class ChartRenderer;
    class GameContext;
    class PlayMenu : public Gx::Image
    {
    public:
        explicit PlayMenu(const GameContext& context);

        void Initialize() override;

        ChartMetadata GetMetadata() const;
        Difficulty GetDifficulty() const;
        void SetMetadata(const ChartMetadata& metadata, const Difficulty diff);

        void SetScoreTracker(const ScoreTracker& scores);
        const ScoreTracker* GetScoreTracker() const;

    private:
        void Update(const double delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        const GameContext& m_context;

        ChartMetadata m_metadata;
        Difficulty m_difficulty;
        const ScoreTracker* m_scoreTracker;

        double m_elapsed = 0;
        ChartRenderer* m_renderer;
        Gx::Label* m_title;
        Gx::Animation* m_playIcon;
        Gx::Image* m_level;
        Gx::BitmapNumber* m_minutes;
        Gx::BitmapNumber* m_seconds;
        Gx::Gauge* m_wave;
        Gx::Gauge* m_bgmVol;
        Gx::Gauge* m_sfxVol;
        std::unordered_map<Accuracy, Gx::BitmapNumber*> m_counters;
    };
}
