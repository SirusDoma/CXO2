#pragma once

#include <CXO2/Core/Chart.hpp>
#include <CXO2/Core/ScoreTracker.hpp>
#include <CXO2/Models/Game.hpp>

#include <CXO2/Network/Events/GameCompletedEventData.hpp>

#include <array>
#include <cstdint>
#include <functional>

namespace Cx
{
    class SessionContext;
    class GameContext
    {
    public:
        using ScoreList = std::array<GameCompletedEventData::ScoreEntry, 8>;

        constexpr static std::size_t MissionCount = 12;

        explicit GameContext(SessionContext& session);

        Chart* GetChart() const;
        void SetChart(Gx::ResourcePtr<Chart> chart);

        Difficulty GetDifficulty() const;
        void SetDifficulty(Difficulty difficulty);

        GameMode GetMode() const;
        void SetMode(GameMode mode);

        float GetSpeed() const;
        void SetSpeed(float speed);

        SpeedMode GetSpeedMode() const;
        void SetSpeedMode(SpeedMode mode);

        std::uint8_t GetMapID() const;
        void SetMapID(std::uint8_t mapID);

        std::uint8_t GetEffectID() const;
        void SetEffectID(std::uint8_t effectID);

        const ScoreList& GetScores() const;
        void SetScores(const ScoreList& scores);

        ScoreTracker& GetScoreTracker();
        const ScoreTracker& GetScoreTracker() const;

        std::int32_t GetMissionID() const;

        std::int32_t GetActiveMissionID() const;
        void SetActiveMissionID(std::int32_t id);

        bool IsMissionEligible() const;

    private:
        std::reference_wrapper<SessionContext> m_session;

        Gx::ResourcePtr<Chart> m_chart;
        Difficulty             m_difficulty{};
        GameMode               m_mode{};
        float                  m_speed{};
        SpeedMode              m_speedMode{SpeedMode::HiSpeed};
        std::uint8_t           m_mapID{};
        std::uint8_t           m_effectID{};
        std::int32_t           m_missionID{-1};

        ScoreList    m_scores{};
        ScoreTracker m_scoreTracker{};
    };
}
