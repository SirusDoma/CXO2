#include <OTwo/Core/Judgements/TimeJudgementStrategy.hpp>
#include <OTwo/Core/ChartRenderer.hpp>

#include <cmath>

TimeJudgementStrategy::TimeJudgementStrategy()
{
    // Base Formula:
    // 1 frame / 60fps = 0.016667 sec per frame (or 16.667ms per frame)
    // Cool: 16.667ms x 2  frames =  33.333ms
    // Good: 16.667ms x 7  frames = 116.667ms
    // Bad:  16.667ms x 15 frames = 250.000ms

    auto evaluator = [] (const ChartRenderer& renderer, const Chart::NoteEvent &ev, const double tolerance)
    {
        const double position = std::abs(ev.Position - renderer.GetRenderPosition());
        const double latency  = position * (60.f / renderer.GetCurrentBPM() * 4.f) * 1000.f;

        return std::abs(latency) <= tolerance;
    };

    Register(Accuracy::Cool, [evaluator] (const auto& renderer, const auto& ev)
    {
        return evaluator(renderer, ev, ((1.f / 60.f) * 3.f) * 1000.f);
    });

    Register(Accuracy::Good, [evaluator] (const auto& renderer, const auto& ev)
    {
        return evaluator(renderer, ev, ((1.f / 60.f) * 10.f) * 1000.f);
    });

    Register(Accuracy::Bad, [evaluator] (const auto& renderer, const auto& ev)
    {
        return evaluator(renderer, ev, ((1.f / 60.f) * 13.f) * 1000.f);
    });
}
