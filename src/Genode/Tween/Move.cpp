#include <Genode/Tween/Move.hpp>

namespace Gx
{
    Move::Move(Transformable& target, const sf::Vector2f position, const sf::Time& duration) :
        m_target(&target),
        m_start(),
        m_end(position),
        m_diff(),
        m_duration(duration)
    {
    }

    void Move::Initialize()
    {
        Task::Initialize();

        m_start  = m_target->GetPosition();
        m_diff   = m_start - m_end;
        m_diff.x = std::abs(m_diff.x);
        m_diff.y = std::abs(m_diff.y);
    }

    void Move::Update(const double delta)
    {
        Task::Update(delta);

        const auto state = GetState();
        if (!m_target || state == TaskState::Stopped || state == TaskState::Completed)
            return;

        float offset = 0.0f;
        auto current = m_target->GetPosition();
        const auto elapsed = GetElapsed();

        if (m_end.x < current.x)
        {
            offset = m_start.x - ((elapsed / m_duration) * m_diff.x);
            if (offset < m_end.x)
                current.x = m_end.x;
            else
                current.x = offset;
        }
        else
        {
            offset = m_start.x + ((elapsed / m_duration) * m_diff.x);
            if (offset > m_end.x)
                current.x = m_end.x;
            else
                current.x = offset;
        }

        if (m_end.y < current.y)
        {
            offset = m_start.y - ((elapsed / m_duration) * m_diff.y);
            if (offset < m_end.y)
                current.y = m_end.y;
            else
                current.y = offset;
        }
        else
        {
            offset = m_start.y + ((elapsed / m_duration) * m_diff.y);
            if (offset > m_end.y)
                current.y = m_end.y;
            else
                current.y = offset;
        }

        m_target->SetPosition(current);
        if (current == m_end)
            Complete();
    }

    void Move::Complete()
    {
        if (GetState() == TaskState::Completed)
            return;

        Task::Complete();
        m_target->SetPosition(m_end);
    }

    void Move::Reset()
    {
        if (GetState() == TaskState::Idle)
            return;

        Task::Reset();
        m_target->SetPosition(m_start);
    }
}
