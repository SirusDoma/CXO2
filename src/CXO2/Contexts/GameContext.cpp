#include <CXO2/Contexts/GameContext.hpp>
#include <CXO2/Contexts/SessionContext.hpp>

#include <algorithm>
#include <limits>

namespace Cx
{
    constexpr std::array<std::int32_t, 99> NextLevelXp = {
        884,     1819,    2839,    3978,    5270,    6749,    8449,    10404,   12648,   15215,
        18139,   21454,   25194,   29393,   34085,   39304,   45084,   51459,   58463,   66130,
        74494,   83589,   93449,   104108,  115600,  127959,  141219,  155414,  170578,  186745,
        203949,  222224,  241604,  262123,  283815,  306714,  330854,  356269,  382993,  411060,
        440504,  471359,  503659,  537438,  572730,  609569,  647989,  688024,  729708,  773075,
        818159,  864994,  913614,  964053,  1016345, 1070524, 1126624, 1184679, 1244723, 1306790,
        1370914, 1437129, 1505469, 1575968, 1648660, 1723579, 1800759, 1880234, 1962038, 2046205,
        2132769, 2221764, 2313224, 2407183, 2503675, 2602734, 2704394, 2808689, 2915653, 3025320,
        3137724, 3252899, 3370879, 3491698, 3615390, 3741989, 3871529, 4004044, 4139568, 4278135,
        4419779, 4564534, 4712434, 4863513, 5017805, 5175344, 5336164, 5500299, 5667783
    };

    GameContext::GameContext(SessionContext& session) :
        m_session(session)
    {
    }

    Chart* GameContext::GetChart() const
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

    std::uint8_t GameContext::GetMapID() const
    {
        return m_mapID;
    }

    void GameContext::SetMapID(const std::uint8_t mapID)
    {
        m_mapID = mapID;
    }

    std::uint8_t GameContext::GetEffectID() const
    {
        return m_effectID;
    }

    void GameContext::SetEffectID(const std::uint8_t effectID)
    {
        m_effectID = effectID;
    }

    const GameContext::ScoreList& GameContext::GetScores() const
    {
        return m_scores;
    }

    void GameContext::SetScores(const ScoreList& scores)
    {
        m_scores = scores;
        std::sort(m_scores.begin(), m_scores.end(), [] (auto& a, auto& b) { return a.Score > b.Score; });
    }

    ScoreTracker& GameContext::GetScoreTracker()
    {
        return m_scoreTracker;
    }

    const ScoreTracker& GameContext::GetScoreTracker() const
    {
        return m_scoreTracker;
    }

    std::int32_t GameContext::GetMissionID() const
    {
        const auto level = m_session.get().GetLevel();
        const auto exp   = m_session.get().GetExperience();

        if (level < 0 || level >= NextLevelXp.size() || exp < NextLevelXp[level] || level % 4 != 3 || level / 4 >= MissionCount)
            return -1;

        return level / 4;
    }

    std::int32_t GameContext::GetActiveMissionID() const
    {
        return m_missionID;
    }

    void GameContext::SetActiveMissionID(std::int32_t id)
    {
        m_missionID = id;
    }

    bool GameContext::IsMissionEligible() const
    {
        if (!m_chart || GetMissionID() == -1)
            return false;

        const auto level     = m_session.get().GetLevel();
        const auto& metadata = m_chart->GetMetadata();
        const auto levelIt   = metadata.Levels.find(m_difficulty);
        const auto noteIt    = metadata.NoteCounts.find(m_difficulty);

        const auto chartLevel = levelIt != metadata.Levels.end() ? levelIt->second : 0u;
        const auto noteCount  = noteIt != metadata.NoteCounts.end() ? noteIt->second : 0u;
        const auto speed      = ToSpeed(m_speed);

        switch (level)
        {
            case 3:  return chartLevel >= 4  && noteCount > 50;
            case 7:  return chartLevel >= 5  && speed == Speed::X05;
            case 11: return chartLevel >= 7;
            case 15: return chartLevel >= 9;
            case 19: return chartLevel >= 11 && noteCount > 375;
            case 23: return chartLevel >= 12;
            case 27: return chartLevel >= 14 && speed == Speed::X60;
            case 31: return chartLevel >= 16 && noteCount > 500;
            case 35: return chartLevel >= 16 && speed == Speed::X10;
            case 39:
            case 43: return chartLevel >= 17;
            case 47: return chartLevel >= 18;
            default: return false;
        }
    }
}
