#include <OTwo/Contexts/GameContext.hpp>

const Chart *GameContext::GetChart() const
{
    return m_chart.get();
}

void GameContext::SetChart(Gx::ResourcePtr<Chart> chart)
{
    m_chart = std::move(chart);
}

GameConfig* GameContext::GetConfig() const
{
    return m_config;
}

void GameContext::SetConfig(GameConfig& config)
{
    m_config = &config;
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

float GameContext::GetSpeed() const
{
    return m_speed;
}

void GameContext::SetSpeed(const float speed)
{
    m_speed = speed;
}

unsigned int GameContext::GetViewport() const
{
    return m_viewport;
}

void GameContext::SetViewport(const unsigned int viewport)
{
    m_viewport = viewport;
}

void GameContext::Reset()
{
    m_chart      = nullptr;
    m_config     = nullptr;
    m_difficulty = Difficulty::EX;
    m_mode       = GameMode::Single;
    m_speed      = 0;
    m_viewport   = 0;
}
