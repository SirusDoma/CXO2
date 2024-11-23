#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    Task::Task() :
        m_state(TaskState::Idle),
        m_elapsed(sf::Time::Zero)
    {
        Task::Reset();
    }

    void Task::Initialize()
    {
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

        switch (m_state)
        {
            case TaskState::Idle:
            {
                Initialize();
                if (m_startCallback)
                    m_startCallback();

                m_state = TaskState::Running;
            }
            case TaskState::Running:
            {
                m_elapsed += sf::milliseconds(delta);
                break;
            }
            default:
                break;
        }
    }

    void Task::Stop()
    {
        if (m_state == TaskState::Stopped)
            return;

        m_state   = TaskState::Stopped;
        m_elapsed = sf::Time::Zero;

        if (m_stopCallback)
            m_stopCallback();
    }

    void Task::Complete()
    {
        if (m_state == TaskState::Completed)
            return;

        m_state   = TaskState::Completed;
        m_elapsed = sf::Time::Zero;

        if (m_completeCallback)
            m_completeCallback();
    }

    void Task::Reset()
    {
        if (m_state == TaskState::Idle)
            return;

        m_state       = TaskState::Idle;
        m_elapsed     = sf::Time::Zero;
    }
}
