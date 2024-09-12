#include <Genode/Fx/Fade.hpp>

namespace Gx
{
    Fade::Fade(Colorable* target, const std::uint8_t opacity, const sf::Time& duration) :
        m_target(target),
        m_start(target->GetColor().a),
        m_end(opacity),
        m_diff(0),
        m_duration(duration)
    {
    }

    void Fade::Update(const double delta)
    {
        Task::Update(delta);

        const auto state = GetState();
        if (!m_target || state == TaskState::Stopped || state == TaskState::Completed)
            return;

        if (state == TaskState::Initial)
        {
            m_start = m_target->GetColor().a;
            m_diff  = std::abs(m_start - m_end);
        }

        short alpha  = 0;
        auto current = m_target->GetColor();
        const auto elapsed = GetElapsed();

        if (m_end < current.a)
        {
            alpha = m_start - ((elapsed / m_duration) * m_diff);
            if (alpha < m_end)
                current.a = m_end;
            else
                current.a = alpha;
        }
        else
        {
            alpha = m_start + ((elapsed / m_duration) * m_diff);
            if (alpha > m_end)
                current.a = m_end;
            else
                current.a = alpha;
        }

        m_target->SetColor(current);
        if (current.a == m_end)
            Complete();
    }
    
    void Fade::Reset()
    {
        Task::Reset();

        auto color = m_target->GetColor();
        color.a = m_start;

        m_target->SetColor(color);
    }
}
