#include <Genode/Tween/Scale.hpp>

namespace Gx
{
    Scale::Scale(Transformable& target, const sf::Vector2f scale, const sf::Time& duration) :
        m_target(&target),
        m_start(target.GetScale()),
        m_end(scale),
        m_diff(),
        m_duration(duration)
    {
    }

    void Scale::Initialize()
    {
        Task::Initialize();

        m_start = m_target->GetScale();
        m_diff = m_start - m_end;
        m_diff.x = std::abs(m_diff.x);
        m_diff.y = std::abs(m_diff.y);
    }

    void Scale::Update(const double delta)
    {
        Task::Update(delta);

        const auto state = GetState();
        if (!m_target || state == TaskState::Stopped || state == TaskState::Completed)
            return;

        float scale  = 0.0f;
        auto current = m_target->GetScale();
        const auto elapsed = GetElapsed();

        if (m_end.x < current.x)
        {
            scale = m_start.x - ((elapsed / m_duration) * m_diff.x);
            if (scale < m_end.x)
                current.x = m_end.x;
            else
                current.x = scale;
        }
        else
        {
            scale = m_start.x + ((elapsed / m_duration) * m_diff.x);
            if (scale > m_end.x)
                current.x = m_end.x;
            else
                current.x = scale;
        }

        if (m_end.y < current.y)
        {
            scale = m_start.y - ((elapsed / m_duration) * m_diff.y);
            if (scale < m_end.y)
                current.y = m_end.y;
            else
                current.y = scale;
        }
        else
        {
            scale = m_start.y + ((elapsed / m_duration) * m_diff.y);
            if (scale > m_end.y)
                current.y = m_end.y;
            else
                current.y = scale;
        }

        m_target->SetScale(current);
        if (current == m_end)
            Complete();
    }

    void Scale::Complete()
    {
        if (GetState() == TaskState::Completed)
            return;

        Task::Complete();
        m_target->SetScale(m_end);
    }

    void Scale::Reset()
    {
        if (GetState() == TaskState::Idle)
            return;

        Task::Reset();
        m_target->SetScale(m_start);
    }
}
