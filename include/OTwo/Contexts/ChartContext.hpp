#ifndef O2JAM_CHART_STATE_HPP
#define O2JAM_CHART_STATE_HPP

#include <OTwo/Chart/Chart.hpp>
#include <OTwo/Models/Game.hpp>
#include <OTwo/Archives/OjmArchive.hpp>

#include <Genode/System/Context.hpp>

class Chart;
class ChartContext : public Gx::Context
{
public:
    ChartContext() = default;

    const Chart *GetChart() const;
    void SetChart(Gx::ResourcePtr<Chart> chart);

    Difficulty GetDifficulty() const;
    void SetDifficulty(Difficulty difficulty);

    GameMode GetMode() const;
    void SetMode(GameMode mode);

private:
    Gx::ResourcePtr<Chart> m_chart;
    Difficulty m_difficulty;
    GameMode m_mode;
    // TODO: Skill modifiers
};

#endif
