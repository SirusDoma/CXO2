#include <Genode/Fx/Rotate.hpp>

namespace Gx
{
    Rotate::Rotate(Transformable* target, float angle, const sf::Time& duration) :
        m_target(target),
        m_start(target->GetRotation()),
        m_diff(0),
        m_end(angle),
        m_duration(duration)
    {
    }

    void Rotate::Update(double delta)
    {
        Task::Update(delta);

        auto state = GetState();
        if (!m_target || state == TaskState::Stopped || state == TaskState::Completed)
            return;

        if (state == TaskState::Initial)
        {
            m_start = m_target->GetRotation();
            m_diff  = abs(m_start - m_end);
        }

        short rotation = 0;
        auto current   = m_target->GetRotation();
        auto elapsed   = GetElapsed();

        if (m_end < current)
        {
            rotation = m_start - ((elapsed / m_duration) * m_diff);
            if (rotation < m_end)
                current = m_end;
            else
                current = rotation;
        }
        else
        {
            rotation = m_start + ((elapsed / m_duration) * m_diff);
            if (rotation > m_end)
                current = m_end;
            else
                current = rotation;
        }

        m_target->SetRotation(current);
        if (current == m_end)
            Complete();
    }

    void Rotate::Reset()
    {
        Task::Reset();
        m_target->SetRotation(m_start);
    }
}