#include <OTwo/Core/ScoreTracker.hpp>

ScoreTracker::ScoreTracker(const Difficulty diff) :
    m_difficulty(diff),
    m_score(0),
    m_maxCombo(0),
    m_combo(0),
    m_maxJamCombo(0),
    m_jamCombo(0),
    m_jamProgress(0)
{
}

void ScoreTracker::Increment(const Chart::NoteEvent& ev, const Accuracy acc, const unsigned int count)
{
    if (acc == Accuracy::None)
        return;

    m_points[acc] += count;
    if (acc == Accuracy::Bad || acc == Accuracy::Miss)
        m_combo  = 0;
    else
        m_combo += count;

    m_maxCombo = m_maxCombo < m_combo && m_combo > 2 ? m_combo : m_maxCombo;

    // TODO: Update score and jam

    if (m_callback)
        m_callback(ev, acc, count);
}

void ScoreTracker::Initialize(const Difficulty diff)
{
    m_difficulty = diff;
    Reset();
}

void ScoreTracker::SetUpdateCallback(const std::function<void(const Chart::NoteEvent &, Accuracy, unsigned int)> &callback)
{
    m_callback = std::move(callback);
}

unsigned int ScoreTracker::GetScore() const
{
    return m_score;
}

unsigned int ScoreTracker::GetPoint(Accuracy acc) const
{
    if (const auto it = m_points.find(acc); it != m_points.end())
        return it->second;

    return 0;
}

unsigned int ScoreTracker::GetMaxCombo() const
{
    return m_maxCombo;
}

unsigned int ScoreTracker::GetCombo() const
{
    return m_combo;
}

unsigned int ScoreTracker::GetMaxJamCombo() const
{
    return m_maxJamCombo;
}

unsigned int ScoreTracker::GetJamCombo() const
{
    return m_jamCombo;
}

float ScoreTracker::GetJamProgress() const
{
    return m_jamProgress;
}

void ScoreTracker::Reset()
{
    m_score       = 0;
    m_maxCombo    = 0;
    m_combo       = 0;
    m_maxJamCombo = 0;
    m_jamCombo    = 0;
    m_jamProgress = 0.f;

    for (auto acc : { Accuracy::Cool, Accuracy::Good, Accuracy::Bad, Accuracy::Miss })
        m_points[acc] = 0;

}
