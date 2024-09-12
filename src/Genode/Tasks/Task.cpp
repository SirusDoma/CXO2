#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    Task::Task() :
        m_state(TaskState::Initial),
        m_elapsed(sf::Time::Zero)
    {
        Reset();
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

    void Task::OnStart(std::function<void()> callback)
    {
        m_onStart = std::move(callback);
    }

    void Task::OnStopped(std::function<void()> callback)
    {
        m_onStop = std::move(callback);
    }

    void Task::OnCompleted(std::function<void()> callback)
    {
        m_onComplete = std::move(callback);
    }

    void Task::Update(const double delta)
    {
        if (m_state == TaskState::Completed || m_state == TaskState::Stopped)
            return;

        if (m_elapsed == sf::Time::Zero && m_onStart)
            m_onStart();
        else if (m_elapsed > sf::Time::Zero && m_state == TaskState::Initial)
            m_state = TaskState::Running;

        m_elapsed += sf::milliseconds(delta);
    }

    void Task::Stop()
    {
        m_state   = TaskState::Stopped;
        m_elapsed = sf::Time::Zero;

        if (m_onStop)
            m_onStop();
    }

    void Task::Complete()
    {
        m_state   = TaskState::Completed;
        m_elapsed = sf::Time::Zero;

        if (m_onComplete)
            m_onComplete();
    }

    void Task::Reset()
    {
        m_state   = TaskState::Initial;
        m_elapsed = sf::Time::Zero;
    }
}
