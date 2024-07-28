#include <OTwo/Contexts/ChartContext.hpp>

const Chart *ChartContext::GetChart() const
{
    return m_chart.get();
}

void ChartContext::SetChart(Gx::ResourcePtr<Chart> chart)
{
    m_chart = std::move(chart);
}

Difficulty ChartContext::GetDifficulty() const
{
    return m_difficulty;
}

void ChartContext::SetDifficulty(const Difficulty difficulty)
{
    m_difficulty = difficulty;
}

GameMode ChartContext::GetMode() const
{
    return m_mode;
}

void ChartContext::SetMode(const GameMode mode)
{
    m_mode = mode;
}
