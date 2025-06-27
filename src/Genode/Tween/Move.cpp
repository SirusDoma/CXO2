#include <Genode/Tween/Move.hpp>

namespace Gx
{
    Move::Move(Transformable& target, const sf::Vector2f position, const sf::Time& duration, MotionFunc motion) :
        Tween(target, duration, std::move(motion)),
        m_start(),
        m_end(position),
        m_diff()
    {
    }

    void Move::Initialize()
    {
        Task::Initialize();

        m_start  = GetTarget().GetPosition();
        m_diff   = m_end - m_start;
    }

    void Move::Update(const double delta)
    {
        Task::Update(delta);

        const auto state = GetState();
        if (state == TaskState::Stopped || state == TaskState::Completed)
            return;

        const auto elapsed = GetElapsed();
        GetTarget().SetPosition(
            m_start.x + (Compute(elapsed / GetDuration()) * m_diff.x),
            m_start.y + (Compute(elapsed / GetDuration()) * m_diff.y)
        );

        if (elapsed >= GetDuration())
            Complete();
    }

    void Move::Complete()
    {
        if (GetState() == TaskState::Completed)
            return;

        Task::Complete();
        GetTarget().SetPosition(m_end);
    }

    void Move::Reset()
    {
        if (GetState() == TaskState::Idle)
            return;

        Task::Reset();
        GetTarget().SetPosition(m_start);
    }
}
