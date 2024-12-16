#include <Genode/Tasks/Scheduler.hpp>

namespace Gx
{
    Scheduler::Scheduler() :
        m_duration(sf::Time::Zero),
        m_interval(sf::Time::Zero),
        m_scheduleDelta(0),
        m_count(0),
        m_repeatCount(0)
    {
    }

    Scheduler::Scheduler(const sf::Time schedule, const std::function<void(const Scheduler&, double)>& update) :
        m_duration(sf::Time::Zero),
        m_interval(schedule),
        m_scheduleDelta(0),
        m_count(0),
        m_repeatCount(0),
        m_callback(update)
    {
    }

    Scheduler::Scheduler(const std::size_t repeatCount, const sf::Time schedule, const std::function<void(const Scheduler&, double)>& update) :
        m_duration(sf::Time::Zero),
        m_interval(schedule),
        m_scheduleDelta(0),
        m_count(0),
        m_repeatCount(repeatCount),
        m_callback(update)
    {
    }

    Scheduler::Scheduler(const sf::Time duration, const sf::Time interval, const std::function<void(const Scheduler&, double)> &update) :
        m_duration(duration),
        m_interval(interval),
        m_scheduleDelta(0),
        m_count(0),
        m_repeatCount(0),
        m_callback(update)
    {
    }

    Scheduler Scheduler::Once(const sf::Time schedule, const std::function<void(const Scheduler&, double)>& update)
    {
        return {1, schedule, update};
    }

    sf::Time Scheduler::GetDuration() const
    {
        return m_duration;
    }

    sf::Time Scheduler::GetInterval() const
    {
        return m_interval;
    }

    void Scheduler::Initialize()
    {
        Task::Initialize();

        m_scheduleDelta = 0;
        m_count = 0;
    }

    void Scheduler::Update(const double delta)
    {
        Task::Update(delta);
        if (GetState() != TaskState::Running)
            return;

        m_scheduleDelta += delta;
        if (m_callback && m_scheduleDelta >= m_interval.asMilliseconds())
        {
            m_callback(*this, m_scheduleDelta);
            m_scheduleDelta = 0;

            if (m_repeatCount != 0)
                m_count++;
        }

        if ((m_repeatCount != 0 && m_count >= m_repeatCount) || (m_duration != sf::Time::Zero && GetElapsed() >= m_duration))
            Complete();
    }

    void Scheduler::Complete()
    {
        Task::Complete();
        if (m_callback)
            m_callback(*this, m_scheduleDelta);
    }

    void Scheduler::Reset()
    {
        Task::Reset();

        m_scheduleDelta = 0;
        m_count = 0;
    }
}
