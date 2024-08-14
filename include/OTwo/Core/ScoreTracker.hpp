#ifndef O2JAM_CORE_SCORE_TRACKER_HPP
#define O2JAM_CORE_SCORE_TRACKER_HPP

#include <OTwo/Core/Chart.hpp>
#include <OTwo/Models/Game.hpp>

#include <Genode/System/Provider.hpp>

#include <functional>
#include <unordered_map>

class ScoreTracker : public Gx::Provider
{
public:

    ScoreTracker() = default;
    explicit ScoreTracker(Difficulty diff);

    virtual Accuracy Increment(const Chart::NoteEvent& ev, Accuracy acc, unsigned int count = 1);

    void Initialize(Difficulty diff = Difficulty::MX);

    void SetIncrementCallback(const std::function<void(const Chart::NoteEvent&, Accuracy, unsigned int)>& callback);
    void SetJamComboCallback(const std::function<void(const Chart::NoteEvent&, Accuracy, unsigned int)>& callback);

    bool IsEnabled() const;

    void SetEnabled(const bool enabled);

    virtual unsigned int GetScore() const;
    unsigned int GetPoint(Accuracy acc) const;

    unsigned int GetMaxCombo() const;
    unsigned int GetCombo() const;

    unsigned int GetMaxJamCombo() const;
    unsigned int GetJamCombo() const;
    virtual unsigned int GetJamProgress() const;

    unsigned int GetBufferCount() const;
    unsigned int GetBufferProgress() const;

    void Reset();

private:
    Difficulty m_difficulty;
    std::function<void(const Chart::NoteEvent&, Accuracy, unsigned int)> m_incrementCallback;
    std::function<void(const Chart::NoteEvent&, Accuracy, unsigned int)> m_jamComboCallback;

    mutable std::unordered_map<Accuracy, unsigned int> m_points;

    bool m_enabled;
    unsigned int m_score;
    unsigned int m_maxCombo;
    unsigned int m_combo;
    unsigned int m_jams;
    unsigned int m_maxJamCombo;
    unsigned int m_jamCombo;
    unsigned int m_jamProgress;
    unsigned int m_buffer;
    unsigned int m_bufferProgress;
};

#endif
