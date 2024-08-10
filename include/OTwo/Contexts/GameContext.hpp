#ifndef O2JAM_GAME_CONTEXT_HPP
#define O2JAM_GAME_CONTEXT_HPP

#include <OTwo/Chart/Chart.hpp>
#include <OTwo/Models/Game.hpp>
#include <OTwo/Archives/OjmArchive.hpp>

#include <Genode/System/Provider.hpp>
#include <OTwo/Config/GameConfig.hpp>

class Chart;
class GameContext : public Gx::Provider
{
public:
    GameContext() = default;

    const Chart *GetChart() const;
    void SetChart(Gx::ResourcePtr<Chart> chart);

    GameConfig *GetConfig() const;
    void SetConfig(GameConfig &config);

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
    Gx::ResourcePtr<Chart> m_chart;
    GameConfig *m_config;
    Difficulty m_difficulty;
    GameMode m_mode;
    float m_speed;
    unsigned int m_viewport;

    // TODO: Skill modifiers
};

#endif
