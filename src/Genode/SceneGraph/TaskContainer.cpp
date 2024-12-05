#include <Genode/SceneGraph/TaskContainer.hpp>
#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    TaskContainer::TaskContainer() :
        m_state(TaskState::Idle)
    {
    }

    void TaskContainer::Stop(const Task& task)
    {
        const auto it = std::find_if(m_tasks.begin(), m_tasks.end(),
        [&](const auto& t)
        {
            return t.get() == &task;
        });

        if (it != m_tasks.end())
        {
            // Run update before deleting
            if ((*it)->GetState() == TaskState::Running)
            {
                (*it)->Update(0);
                (*it)->Stop();
            }

            m_tasks.erase(it);
        }
    }

    void TaskContainer::StopAll()
    {
        for (const auto& task : m_tasks)
        {
            // Run update before deleting
            if (task->GetState() == TaskState::Running)
            {
                task->Update(0);
                task->Stop();
            }
        }

        m_tasks.clear();
    }

    void TaskContainer::Update(const double delta)
    {
        // Tasks can be added or removed to/from the list during the update
        // The container need to guarantee that:
        //   1. The newly added tasks gets updated
        //   2. The non-running tasks are removed
        //   3. Every task is updated only once

        // This will reject nested Update which prevents:
        //   1. Updating task more than once in a single frame
        //   2. Removing non-running tasks that past the pointed task in the root Update loop
        if (m_state == TaskState::Running)
            return;

        m_state = TaskState::Running;
        for (std::size_t i = 0; i < m_tasks.size();)
        {
            // Any task may add new tasks into the list.
            // The newly added tasks will be processed within this frame too.
            m_tasks[i]->Update(delta);
            if (m_tasks[i]->GetState() == TaskState::Stopped || m_tasks[i]->GetState() == TaskState::Completed)
                m_tasks.erase(m_tasks.begin() + i);
            else
                ++i;
        }

        m_state = TaskState::Idle;
    }
}
