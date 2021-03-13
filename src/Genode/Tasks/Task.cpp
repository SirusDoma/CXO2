#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    Task::Task() :
        m_state(TaskState::Initial),
        m_elapsed(0)
    {
        Reset();
    }

    Task::~Task()
    {
    }

    Task::TaskState Task::GetState() const
    {
        return m_state;
    }

    double Task::GetElapsed() const
    {
        return m_elapsed;
    }

    void Task::OnStart(std::function<void()> callback)
    {
        m_start = callback;
    }

    void Task::OnStopped(std::function<void()> callback)
    {
        m_stop = callback;
    }

    void Task::OnCompleted(std::function<void()> callback)
    {
        m_complete = callback;
    }

    void Task::Update(double delta)
    {
        if (m_state == TaskState::Completed || m_state == TaskState::Stopped)
            return;

        if (m_elapsed == 0 && m_start)
            m_start();
        else if (m_elapsed > 0 && m_state == TaskState::Initial)
            m_state = TaskState::Running;

        m_elapsed += delta;
    }

    void Task::Stop()
    {
        m_state   = TaskState::Stopped;
        m_elapsed = 0;

        if (m_stop)
            m_stop();
    }

    void Task::Complete()
    {
        m_state   = TaskState::Completed;
        m_elapsed = 0;

        if (m_complete)
            m_complete();
    }

    void Task::Reset()
    {
        m_state   = TaskState::Initial;
        m_elapsed = 0;
    }
}