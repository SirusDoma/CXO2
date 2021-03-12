#include <Genode/Tasks/Delay.hpp>

namespace Gx
{
    Delay::Delay() :
        m_duration(0)
    {
    }

    Delay::Delay(double duration) :
        m_duration(duration)
    {
    }

    Delay::~Delay()
    {
    }

    double Delay::GetDuration() const
    {
        return m_duration;
    }

    void Delay::SetDuration(double duration)
    {
        m_duration = duration;
    }

    void Delay::Update(double delta)
    {
        Task::Update(delta);

        if (GetState() != TaskState::Running)
            return;

        if (GetElapsed() >= GetDuration())
            Complete();
    }
}