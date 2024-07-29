#include <OTwo/Contexts/GameContext.hpp>

const Chart *GameContext::GetChart() const
{
    return m_chart.get();
}

void GameContext::SetChart(Gx::ResourcePtr<Chart> chart)
{
    m_chart = std::move(chart);
}

Difficulty GameContext::GetDifficulty() const
{
    return m_difficulty;
}

void GameContext::SetDifficulty(const Difficulty difficulty)
{
    m_difficulty = difficulty;
}

GameMode GameContext::GetMode() const
{
    return m_mode;
}

void GameContext::SetMode(const GameMode mode)
{
    m_mode = mode;
}
