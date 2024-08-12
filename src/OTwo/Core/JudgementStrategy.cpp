#include <OTwo/Core/JudgementStrategy.hpp>
#include <OTwo/Core/ChartRenderer.hpp>

void JudgementStrategy::Initialize(const ChartRenderer &renderer)
{
    m_renderer = &renderer;
}

Judgement JudgementStrategy::Judge(const Chart::NoteEvent &ev) const
{
    if (!m_renderer)
        throw Gx::Exception("JudgementStrategy is not initialized!");

    const double latency = ev.Position - m_renderer->GetRenderPosition();
    for (auto acc : {Accuracy::Cool, Accuracy::Good, Accuracy::Bad})
    {
        if (auto it = m_evaluators.find(acc); it != m_evaluators.end() && it->second(*m_renderer, ev))
            return Judgement{acc, latency};
    }

    if (latency >= 0)
        return Judgement{Accuracy::None, latency};

    return Judgement{Accuracy::Miss, latency};
}

void JudgementStrategy::Register(const Accuracy accuracy, const JudgementEvaluator &evaluator)
{
    if (accuracy == Accuracy::None)
        return;

    m_evaluators[accuracy] = std::move(evaluator);
}
