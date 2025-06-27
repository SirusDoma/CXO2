#include <Genode/Tween/Rotate.hpp>

namespace Gx
{
    Rotate::Rotate(Transformable& target, const float angle, const sf::Time& duration, MotionFunc motion) :
        Tween(target, duration, std::move(motion)),
        m_start(target.GetRotation().asDegrees()),
        m_end(angle),
        m_diff(0)
    {
    }

    void Rotate::Initialize()
    {
        Task::Initialize();

        m_start = GetTarget().GetRotation().asDegrees();
        m_diff  = m_end - m_start;
    }

    void Rotate::Update(const double delta)
    {
        Task::Update(delta);

        const auto state = GetState();
        if (state == TaskState::Stopped || state == TaskState::Completed)
            return;

        const auto elapsed   = GetElapsed();
        GetTarget().SetRotation(
            m_start + (Compute(elapsed / GetDuration()) * m_diff)
        );

        if (elapsed >= GetDuration())
            Complete();
    }

    void Rotate::Complete()
    {
        if (GetState() == TaskState::Completed)
            return;

        Task::Complete();
        GetTarget().SetRotation(m_end);
    }

    void Rotate::Reset()
    {
        if (GetState() == TaskState::Idle)
            return;

        Task::Reset();
        GetTarget().SetRotation(m_start);
    }
}
