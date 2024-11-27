#include <Genode/Tween/Rotate.hpp>

namespace Gx
{
    Rotate::Rotate(Transformable& target, const float angle, const sf::Time& duration) :
        m_target(&target),
        m_start(target.GetRotation().asDegrees()),
        m_end(angle),
        m_diff(0),
        m_duration(duration)
    {
    }

    void Rotate::Initialize()
    {
        Task::Initialize();

        m_start = m_target->GetRotation().asDegrees();
        m_diff  = std::abs(m_start - m_end);
    }

    void Rotate::Update(const double delta)
    {
        Task::Update(delta);

        const auto state = GetState();
        if (!m_target || state == TaskState::Stopped || state == TaskState::Completed)
            return;

        short rotation = 0;
        auto current   = m_target->GetRotation().asDegrees();
        const auto elapsed   = GetElapsed();

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

    void Rotate::Complete()
    {
        if (GetState() == TaskState::Completed)
            return;

        Task::Complete();
        m_target->SetRotation(m_end);
    }

    void Rotate::Reset()
    {
        if (GetState() == TaskState::Idle)
            return;

        Task::Reset();
        m_target->SetRotation(m_start);
    }
}
