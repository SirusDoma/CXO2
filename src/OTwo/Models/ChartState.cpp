#include <OTwo/Models/ChartState.hpp>
#include <OTwo/Models/Game.hpp>

const Chart *ChartState::GetChart() const
{
    return m_chart.get();
}

void ChartState::SetChart(Gx::ResourcePtr<Chart> chart)
{
    m_chart = std::move(chart);
}

Difficulty ChartState::GetDifficulty() const
{
    return m_difficulty;
}

void ChartState::SetDifficulty(const Difficulty difficulty)
{
    m_difficulty = difficulty;
}

GameMode ChartState::GetMode() const
{
    return m_mode;
}

void ChartState::SetMode(const GameMode mode)
{
    m_mode = mode;
}
