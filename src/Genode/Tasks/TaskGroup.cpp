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

        if (m_tasks.size() == 0)
            return Complete();

        if (GetState() != TaskState::Running)
            return;

        bool completed = true;
        for (size_t i = 0; i < m_tasks.size(); ++i)
        {
            m_tasks[i]->Update(delta);
            completed = completed && m_tasks[i]->GetState() == TaskState::Completed;
        }

        if (completed)
            return Complete();
    }

    void TaskGroup::Stop()
    {
        if (GetState() == TaskState::Stopped)
            return;

        Task::Stop();
        for (size_t i = 0; i < m_tasks.size(); ++i)
            m_tasks[i]->Stop();
    }

    void TaskGroup::Complete()
    {
        if (GetState() == TaskState::Completed)
            return;

        Task::Complete();
        for (size_t i = 0; i < m_tasks.size(); ++i)
            m_tasks[i]->Complete();
    }

    void TaskGroup::Reset()
    {
        if (GetState() == TaskState::Idle)
            return;

        Task::Reset();
        for (size_t i = 0; i < m_tasks.size(); ++i)
            m_tasks[i]->Reset();
    }
}
