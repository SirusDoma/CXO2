#include <OTwo/Core/ScoreTracker.hpp>

ScoreTracker::ScoreTracker(const Difficulty diff) :
    m_difficulty(diff),
    m_enabled(true),
    m_maxCombo(0),
    m_combo(0),
    m_jams(0),
    m_maxJamCombo(0),
    m_jamCombo(0),
    m_jamProgress(0),
    m_buffer(0),
    m_bufferProgress(0)
{
}

Accuracy ScoreTracker::Increment(const Chart::NoteEvent& ev, Accuracy acc, unsigned int count)
{
    if (acc == Accuracy::None || !m_enabled)
    {
        if (!m_enabled)
            acc = Accuracy::None;

        return acc;
    }

    if (acc == Accuracy::Bad && m_buffer > 0)
    {
        m_bufferProgress = 0;
        if (count > m_buffer)
        {
            count -= m_buffer;
            Increment(ev, Accuracy::Cool, m_buffer);
        }
        else
            acc = Accuracy::Cool;

        m_buffer -= count;
    }

    m_points[acc] += count;
    if (acc == Accuracy::Cool || acc == Accuracy::Good)
    {
        m_combo += count;
        if (acc == Accuracy::Cool)
        {
            m_bufferProgress += count;
            m_jamProgress += 4 * count;
        }
        else
        {
            m_bufferProgress = 0;
            m_jamProgress += 2 * count;
        }

        // Buffer
        if (m_bufferProgress >= 15)
        {
            m_buffer = std::min<unsigned int>(m_buffer + 1, 5);
            m_bufferProgress = 0;
        }

        // Jam Combo
        if (m_jamProgress >= 100)
        {
            m_jamProgress %= 100;
            m_jamCombo++;
            m_jams++;

            for (auto callback : m_jamComboCallbacks)
            {
                if (callback)
                    callback(ev, acc, m_jamCombo);
            }
        }
    }
    else
    {
        m_combo = 0;
        m_bufferProgress = 0;
        m_jamProgress = 0;
        m_jamCombo = 0;
    }

    m_maxCombo = m_maxCombo < GetCombo() ? GetCombo() : m_maxCombo;
    m_maxJamCombo = std::max(m_maxJamCombo, m_jamCombo);

    for (auto callback : m_incrementCallbacks)
    {
        if (callback)
            callback(ev, acc, count);
    }

    return acc;
}

void ScoreTracker::Initialize(const Difficulty diff)
{
    m_enabled    = true;
    m_difficulty = diff;
    Reset();
}

void ScoreTracker::AddIncrementListener(const ScoreCallback& callback)
{
    m_incrementCallbacks.push_back(std::move(callback));
}

void ScoreTracker::AddJamComboListener(const ScoreCallback& callback)
{
    m_jamComboCallbacks.push_back(std::move(callback));
}

bool ScoreTracker::IsEnabled() const
{
    return m_enabled;
}

void ScoreTracker::SetEnabled(const bool enabled)
{
    m_enabled = enabled;
}

unsigned long long ScoreTracker::GetScorePoint() const
{
    if (std::all_of(m_points.begin(), m_points.end(), [this] (const auto& p) { return p.first == Accuracy::Miss || p.second == 0; }))
        return 0;

    const long long score = m_points[Accuracy::Cool] * (200 + 10 * m_jams) +
                            m_points[Accuracy::Good] * (100 + 5  * m_jams) +
                            m_points[Accuracy::Bad]  * 4 -
                            m_points[Accuracy::Miss] * 10;

    return std::max<unsigned long long>(score, 0);
}

unsigned long long ScoreTracker::GetPoint(const Accuracy acc) const
{
    if (const auto it = m_points.find(acc); it != m_points.end())
        return it->second;

    return 0;
}

unsigned long long ScoreTracker::GetMaxCombo() const
{
    return m_maxCombo;
}

unsigned long long ScoreTracker::GetCombo() const
{
    return m_combo > 0 ? m_combo - 1 : m_combo;
}

unsigned long long ScoreTracker::GetMaxJamCombo() const
{
    return m_maxJamCombo;
}

unsigned long long ScoreTracker::GetJamCombo() const
{
    return m_jamCombo;
}

unsigned int ScoreTracker::GetJamProgress() const
{
    return m_jamProgress;
}

unsigned int ScoreTracker::GetBufferCount() const
{
    return m_buffer;
}

unsigned int ScoreTracker::GetBufferProgress() const
{
    return m_bufferProgress;
}

void ScoreTracker::Reset()
{
    m_maxCombo       = 0;
    m_combo          = 0;
    m_maxJamCombo    = 0;
    m_jamCombo       = 0;
    m_jamProgress    = 0;
    m_buffer         = 0;
    m_bufferProgress = 0;

    m_incrementCallbacks.clear();
    m_jamComboCallbacks.clear();
    for (auto acc : { Accuracy::Cool, Accuracy::Good, Accuracy::Bad, Accuracy::Miss })
        m_points[acc] = 0;

}
