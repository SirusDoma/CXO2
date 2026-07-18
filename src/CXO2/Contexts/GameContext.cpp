#include <CXO2/Contexts/GameContext.hpp>

namespace Cx
{
    GameContext::GameContext(GameConfig& config) :
        m_config(config),
        m_difficulty(),
        m_mode(),
        m_speed(),
        m_mapID(),
        m_effectID(),
        m_viewport(),
        m_scoreEntries()
    {
    }

    GameConfig& GameContext::GetConfig() const
    {
        return m_config;
    }

    const Chart* GameContext::GetChart() const
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

    float GameContext::GetSpeed() const
    {
        return m_speed;
    }

    void GameContext::SetSpeed(const float speed)
    {
        m_speed = speed;
    }

    SpeedMode GameContext::GetSpeedMode() const
    {
        return m_speedMode;
    }

    void GameContext::SetSpeedMode(const SpeedMode mode)
    {
        m_speedMode = mode;
    }

    unsigned int GameContext::GetMapID() const
    {
        return m_mapID;
    }

    void GameContext::SetMapID(const unsigned int mapID)
    {
        m_mapID = mapID;
    }

    unsigned int GameContext::GetEffectID() const
    {
        return m_effectID;
    }

    void GameContext::SetEffectID(const unsigned int effectID)
    {
        m_effectID = effectID;
    }

    unsigned int GameContext::GetViewport() const
    {
        return m_viewport;
    }

    void GameContext::SetViewport(const unsigned int viewport)
    {
        m_viewport = viewport;
    }

    const std::array<ScoreEntry, 8>& GameContext::GetScoreEntries() const
    {
        return m_scoreEntries;
    }

    void GameContext::SetScoreEntries(const std::array<ScoreEntry, 8>& entries)
    {
        m_scoreEntries = entries;
        std::sort(m_scoreEntries.begin(), m_scoreEntries.end(), [] (auto& a, auto& b) { return a.Score > b.Score; });
    }

    void GameContext::Reset()
    {
        m_chart      = nullptr;
        m_difficulty = Difficulty::EX;
        m_mode       = GameMode::Single;
        m_speed      = 0;
        m_speedMode  = SpeedMode::HiSpeed;
        m_mapID      = 0;
        m_effectID   = 0;
        m_viewport   = 0;
    }
}
