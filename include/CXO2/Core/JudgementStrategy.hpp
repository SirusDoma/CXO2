#pragma once

#include <CXO2/Core/Chart.hpp>
#include <CXO2/Models/Game.hpp>

#include <functional>
#include <unordered_map>

namespace Cx
{
    struct Judgement
    {
        Accuracy Accuracy;
        double   Latency;
    };

    class ChartRenderer;
    class JudgementStrategy
    {
    public:
        using JudgementEvaluator = std::function<bool(const ChartRenderer&, const Chart::NoteEvent& ev)>;

        JudgementStrategy() = default;
        virtual ~JudgementStrategy() = default;

        virtual void Initialize(const ChartRenderer& renderer);
        virtual Judgement Judge(const Chart::NoteEvent& ev) const;

    protected:
        void Register(Accuracy accuracy, const JudgementEvaluator& evaluator);

    private:
        const ChartRenderer* m_renderer;
        std::unordered_map<Accuracy, JudgementEvaluator> m_evaluators;
    };
}
