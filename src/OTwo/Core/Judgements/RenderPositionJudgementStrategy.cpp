#include <OTwo/Core/Judgements/RenderPositionJudgementStrategy.hpp>
#include <OTwo/Core/ChartRenderer.hpp>

#include <cmath>

RenderPositionJudgementStrategy::RenderPositionJudgementStrategy()
{
    // O2Jam Judgement
    //
    // Cool: +/- 06 (0x06)
    // Good: +/- 18 (0x12)
    // Bad:
    // - Tap / Hold: +/- 25 (0x19)
    // - Release:    +/- 24 (0x18)

    constexpr double MaxNotesPerMeasure = 192.f;

    auto evaluator = [=] (const ChartRenderer& renderer, const Chart::NoteEvent& ev, double tolerance)
    {
        const double inputPosition = renderer.GetRenderPosition();
        const double latency       = ev.Position - inputPosition;

        tolerance /= MaxNotesPerMeasure;
        return std::abs(latency) <= tolerance;
    };

    Register(Accuracy::Cool, [=] (const auto& renderer, const auto& ev)
    {
        return evaluator(renderer, ev, 06);
    });

    Register(Accuracy::Good, [=] (const auto& renderer, const auto& ev)
    {
        return evaluator(renderer, ev, 18);
    });

    Register(Accuracy::Bad, [=] (const auto& renderer, const auto& ev)
    {
        if (ev.Type == Chart::NoteType::Release)
            return evaluator(renderer, ev, 24);

        return evaluator(renderer, ev, 25);
    });
}
