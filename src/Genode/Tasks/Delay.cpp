#include <Genode/Tasks/Delay.hpp>

namespace Gx
{
    Delay::Delay() :
        m_duration(sf::Time::Zero)
    {
    }

    Delay::Delay(const sf::Time duration) :
        m_duration(duration)
    {
    }

    Delay::Delay(const sf::Time duration, const std::function<void()>& callback) :
        m_duration(duration)
    {
        SetCompletedCallback(callback);
    }

    sf::Time Delay::GetDuration() const
    {
        return m_duration;
    }

    void Delay::SetDuration(const sf::Time& duration)
    {
        m_duration = duration;
    }

    void Delay::Update(const double delta)
    {
        Task::Update(delta);

        if (GetState() != TaskState::Running)
            return;

        if (GetElapsed() >= GetDuration())
            Complete();
    }
}
