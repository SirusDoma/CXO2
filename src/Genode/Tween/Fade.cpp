#include <Genode/Tween/Fade.hpp>

namespace Gx
{
    Fade::Fade(Colorable& target, const std::uint8_t opacity, const sf::Time& duration, MotionFunc motion) :
        Tween(target, duration, std::move(motion)),
        m_start(target.GetColor().a),
        m_end(opacity),
        m_diff(0)
    {
    }

    void Fade::Initialize()
    {
        Task::Initialize();

        m_start = GetTarget().GetColor().a;
        m_diff  = static_cast<std::int16_t>(std::abs(m_start - m_end));
    }

    void Fade::Update(const double delta)
    {
        Task::Update(delta);

        const auto state = GetState();
        if (state == TaskState::Stopped || state == TaskState::Completed)
            return;

        std::int16_t alpha  = 0;
        auto current = GetTarget().GetColor();
        const auto elapsed = GetElapsed();

        if (m_end < current.a)
        {
            alpha = m_start - (Compute(elapsed / GetDuration()) * m_diff);
            if (alpha < m_end)
                current.a = m_end;
            else
                current.a = alpha;
        }
        else
        {
            alpha = m_start + (Compute(elapsed / GetDuration()) * m_diff);
            if (alpha > m_end)
                current.a = m_end;
            else
                current.a = alpha;
        }

        GetTarget().SetColor(current);
        if (GetElapsed() >= GetDuration())
            Complete();
    }

    void Fade::Complete()
    {
        if (GetState() == TaskState::Completed)
            return;

        Task::Complete();

        auto color = GetTarget().GetColor();
        color.a = m_end;

        GetTarget().SetColor(color);
    }

    void Fade::Reset()
    {
        if (GetState() == TaskState::Idle)
            return;

        Task::Reset();

        auto color = GetTarget().GetColor();
        color.a = m_start;

        GetTarget().SetColor(color);
    }
}
