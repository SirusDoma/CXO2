#ifndef O2JAM_GAME_CONTEXT_HPP
#define O2JAM_GAME_CONTEXT_HPP

#include <OTwo/Core/Chart.hpp>
#include <OTwo/Models/Game.hpp>
#include <OTwo/Archives/OjmArchive.hpp>

#include <OTwo/Config/GameConfig.hpp>

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

    unsigned int GetViewport() const;
    void SetViewport(unsigned int viewport);

    void Reset();

private:
    GameConfig& m_config;

    Gx::ResourcePtr<Chart> m_chart;
    Difficulty m_difficulty;
    GameMode m_mode;
    float m_speed;
    unsigned int m_viewport;

    // TODO: Skill modifiers
};

#endif
