#include <Genode/Tasks/TaskGroup.hpp>

namespace Gx
{
    TaskGroup& TaskGroup::Add(Task& task)
    {
        m_tasks.push_back(
            ResourcePtr<Task>(&task, [] (auto _) {})
        );

        return *this;
    }

    TaskGroup& TaskGroup::Remove(const Task& task)
    {
        m_tasks.erase(std::remove_if(m_tasks.begin(), m_tasks.end(), [&](const auto& t)
            {
                return t.get() == &task;
            }), m_tasks.end()
        );

        return *this;
    }

    void TaskGroup::Update(const double delta)
    {
        Task::Update(delta);

        if (m_tasks.empty())
            return Complete();

        if (GetState() != TaskState::Running)
            return;

        bool completed = true;
        for (const auto& m_task : m_tasks)
        {
            m_task->Update(delta);
            completed = completed && m_task->GetState() == TaskState::Completed;
        }

        if (completed)
            return Complete();
    }

    void TaskGroup::Stop()
    {
        if (GetState() == TaskState::Stopped)
            return;

        Task::Stop();
        for (const auto& m_task : m_tasks)
            m_task->Stop();
    }

    void TaskGroup::Complete()
    {
        if (GetState() == TaskState::Completed)
            return;

        Task::Complete();
        for (const auto& m_task : m_tasks)
            m_task->Complete();
    }

    void TaskGroup::Reset()
    {
        if (GetState() == TaskState::Idle)
            return;

        Task::Reset();
        for (const auto & m_task : m_tasks)
            m_task->Reset();
    }
}
