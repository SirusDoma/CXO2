#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    Task::Task() :
        m_state(TaskState::Initial),
        m_elapsed(sf::Time::Zero)
    {
        Task::Reset();
    }

    TaskState Task::GetState() const
    {
        return m_state;
    }

    void Task::SetState(const TaskState& state)
    {
        m_state = state;
    }

    const sf::Time& Task::GetElapsed() const
    {
        return m_elapsed;
    }

    void Task::SetStartCallback(const std::function<void()>& callback)
    {
        m_startCallback = callback;
    }

    void Task::SetStoppedCallback(const std::function<void()>& callback)
    {
        m_stopCallback = callback;
    }

    void Task::SetCompletedCallback(const std::function<void()>& callback)
    {
        m_completeCallback = callback;
    }

    void Task::Update(const double delta)
    {
        if (m_state == TaskState::Completed || m_state == TaskState::Stopped)
            return;

        if (m_elapsed == sf::Time::Zero && m_startCallback)
            m_startCallback();
        else if (m_elapsed > sf::Time::Zero && m_state == TaskState::Initial)
            m_state = TaskState::Running;

        m_elapsed += sf::milliseconds(delta);
    }

    void Task::Stop()
    {
        m_state   = TaskState::Stopped;
        m_elapsed = sf::Time::Zero;

        if (m_stopCallback)
            m_stopCallback();
    }

    void Task::Complete()
    {
        m_state   = TaskState::Completed;
        m_elapsed = sf::Time::Zero;

        if (m_completeCallback)
            m_completeCallback();
    }

    void Task::Reset()
    {
        m_state   = TaskState::Initial;
        m_elapsed = sf::Time::Zero;
    }
}
