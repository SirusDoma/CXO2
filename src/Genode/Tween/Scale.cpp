#include <Genode/Tween/Scale.hpp>
#include <utility>

namespace Gx
{
    Scale::Scale(Transformable& target, const sf::Vector2f scale, const sf::Time& duration, MotionFunc motion) :
        Tween(target, duration, std::move(motion)),
        m_start(target.GetScale()),
        m_end(scale),
        m_diff()
    {
    }

    void Scale::Initialize()
    {
        Task::Initialize();

        m_start = GetTarget().GetScale();
        m_diff = m_end - m_start;
    }

    void Scale::Update(const double delta)
    {
        Task::Update(delta);

        const auto state = GetState();
        if (state == TaskState::Stopped || state == TaskState::Completed)
            return;

        const auto elapsed = GetElapsed();
        GetTarget().SetScale(
            m_start.x + (Compute(elapsed / GetDuration()) * m_diff.x),
            m_start.y + (Compute(elapsed / GetDuration()) * m_diff.y)
        );

        if (elapsed >= GetDuration())
            Complete();
    }

    void Scale::Complete()
    {
        if (GetState() == TaskState::Completed)
            return;

        Task::Complete();
        GetTarget().SetScale(m_end);
    }

    void Scale::Reset()
    {
        if (GetState() == TaskState::Idle)
            return;

        Task::Reset();
        GetTarget().SetScale(m_start);
    }
}
