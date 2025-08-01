#pragma once


#include <OTwo/Core/Chart.hpp>

#include <OTwo/Models/Game.hpp>

#include <OTwo/Messages/ScoreEntry.hpp>

#include <OTwo/Config/GameConfig.hpp>

#include <array>

class Chart;
class GameContext
{
public:
    explicit GameContext(GameConfig &config);

    GameConfig& GetConfig() const;

    const Chart* GetChart() const;
    void SetChart(Gx::ResourcePtr<Chart> chart);

    Difficulty GetDifficulty() const;
    void SetDifficulty(Difficulty difficulty);

    GameMode GetMode() const;
    void SetMode(GameMode mode);

    float GetSpeed() const;
    void SetSpeed(float speed);

    unsigned int GetMapID() const;
    void SetMapID(const unsigned int mapID);

    unsigned int GetEffectID() const;
    void SetEffectID(const unsigned int effectID);

    unsigned int GetViewport() const;
    void SetViewport(unsigned int viewport);

    const std::array<ScoreEntry, 8>& GetScoreEntries() const;
    void SetScoreEntries(const std::array<ScoreEntry, 8>& entries);

    void Reset();

private:
    GameConfig& m_config;

    Gx::ResourcePtr<Chart> m_chart;
    Difficulty m_difficulty;
    GameMode m_mode;
    float m_speed;
    unsigned int m_mapID;
    unsigned int m_effectID;
    unsigned int m_viewport;

    // TODO: Skill modifiers

    std::array<ScoreEntry, 8> m_scoreEntries;
};
