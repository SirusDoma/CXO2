#include <OTwo/Core/Judgements/TimeJudgementStrategy.hpp>
#include <OTwo/Core/ChartRenderer.hpp>

#include <cmath>

TimeJudgementStrategy::TimeJudgementStrategy()
{
    // Base Formula:
    // 1 frame / 60fps = 0.016667 sec per frame (or 16.667ms per frame)
    constexpr double FramePerMilliseconds = (1.f / 60.f) * 1000.f;

    auto evaluator = [] (const ChartRenderer& renderer, const Chart::NoteEvent& ev, const double tolerance)
    {
        const double position = std::abs(ev.Position - renderer.GetRenderPosition());
        const double latency  = position * 4.f * (60.f / renderer.GetCurrentBPM()) * 1000.f;

        return std::abs(latency) <= tolerance;
    };

    Register(Accuracy::Cool, [=] (const auto& renderer, const auto& ev)
    {
        return evaluator(renderer, ev, FramePerMilliseconds * 3.2f);
    });

    Register(Accuracy::Good, [=] (const auto& renderer, const auto& ev)
    {
        return evaluator(renderer, ev, FramePerMilliseconds * 8.f);
    });

    Register(Accuracy::Bad, [=] (const auto& renderer, const auto& ev)
    {
        return evaluator(renderer, ev, FramePerMilliseconds * 15.f);
    });
}
