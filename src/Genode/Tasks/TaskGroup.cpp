#include <Genode/Tasks/TaskGroup.hpp>
#include <algorithm>

namespace Gx
{
    TaskGroup* TaskGroup::Add(Task& task)
    {
        m_tasks.push_back(&task);
        return this;
    }

    TaskGroup* TaskGroup::Remove(const Task& task)
    {
        const auto iterator = std::find(m_tasks.begin(), m_tasks.end(), &task);
        if (iterator != m_tasks.end())
            m_tasks.erase(iterator);

        return this;
    }

    void TaskGroup::Update(const double delta)
    {
        Task::Update(delta);

        if (m_tasks.size() == 0)
            return Complete();

        if (GetState() != TaskState::Running)
            return;

        bool completed = true;
        for (const auto task : m_tasks)
        {
            task->Update(delta);
            completed = completed && task->GetState() == TaskState::Completed;
        }

        if (completed)
            return Complete();
    }

    void TaskGroup::Stop()
    {
        Task::Stop();

        for (const auto task : m_tasks)
            task->Stop();
    }

    void TaskGroup::Complete()
    {
        Task::Complete();

        for (const auto task : m_tasks)
            task->Complete();
    }

    void TaskGroup::Reset()
    {
        Task::Reset();

        for (const auto task : m_tasks)
            task->Reset();
    }
}
