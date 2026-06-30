#pragma once

#include <CXO2/Core/Chart.hpp>
#include <CXO2/Models/Game.hpp>

#include <functional>
#include <unordered_map>

namespace Cx
{
    class ScoreTracker
    {
    public:
        using ScoreCallback = std::function<void(const Chart::NoteEvent&, Accuracy, unsigned long long)>;

        ScoreTracker() = default;
        explicit ScoreTracker(Difficulty diff);
        virtual ~ScoreTracker() = default;

        virtual Accuracy Increment(const Chart::NoteEvent& ev, Accuracy acc, unsigned int count = 1);

        void Initialize(Difficulty diff = Difficulty::MX);

        void AddIncrementListener(const ScoreCallback& callback);
        void AddJamComboListener(const ScoreCallback& callback);

        bool IsEnabled() const;

        void SetEnabled(const bool enabled);

        virtual unsigned long long GetScorePoint() const;
        unsigned long long GetPoint(Accuracy acc) const;

        unsigned long long GetMaxCombo() const;
        unsigned long long GetCombo() const;

        unsigned long long GetMaxJamCombo() const;
        unsigned long long GetJamCombo() const;
        virtual unsigned int GetJamProgress() const;

        unsigned int GetBufferCount() const;
        unsigned int GetBufferProgress() const;

        void Reset();

    private:
        Difficulty m_difficulty;
        std::vector<ScoreCallback> m_incrementCallbacks;
        std::vector<ScoreCallback> m_jamComboCallbacks;

        mutable std::unordered_map<Accuracy, unsigned int> m_points;

        bool m_enabled;
        unsigned long long m_capturedScore;
        unsigned long long m_maxCombo;
        unsigned long long m_combo;
        unsigned long long m_jams;
        unsigned long long m_maxJamCombo;
        unsigned long long m_jamCombo;
        unsigned int m_jamProgress;
        unsigned int m_buffer;
        unsigned int m_bufferProgress;
    };
}
