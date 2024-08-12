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

    virtual void Increment(const Chart::NoteEvent& ev, Accuracy acc, unsigned int count = 1);

    void Initialize(Difficulty diff = Difficulty::MX);
    void SetUpdateCallback(const std::function<void(const Chart::NoteEvent&, Accuracy, unsigned int)>& callback);

    virtual unsigned int GetScore() const;
    unsigned int GetPoint(Accuracy acc) const;

    unsigned int GetMaxCombo() const;
    unsigned int GetCombo() const;

    unsigned int GetMaxJamCombo() const;
    unsigned int GetJamCombo() const;
    virtual float GetJamProgress() const;

    void Reset();

private:
    Difficulty m_difficulty;
    std::function<void(const Chart::NoteEvent&, Accuracy, unsigned int)> m_callback;
    std::unordered_map<Accuracy, unsigned int> m_points;

    unsigned int m_score;
    unsigned int m_maxCombo;
    unsigned int m_combo;
    unsigned int m_maxJamCombo;
    unsigned int m_jamCombo;
    float m_jamProgress;
};

#endif
