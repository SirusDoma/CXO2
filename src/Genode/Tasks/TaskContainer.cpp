#include <Genode/Tasks/Task.hpp>
#include <Genode/Tasks/TaskContainer.hpp>

namespace Gx
{
    TaskContainer::TaskContainer() :
        m_tasks()
    {
    }

    void TaskContainer::Run(Task& task)
    {
        m_tasks.push_back(&task);
    }

    void TaskContainer::Stop(Task& task)
    {
        const auto iterator = std::find_if(m_tasks.begin(), m_tasks.end(), [task](auto t) { return &task == t; });
        if (iterator != m_tasks.end())
        {
            // Run update before deleting
            const auto item = *iterator;
            if (item->GetState() != TaskState::Completed)
            {
                item->Stop();
                item->Update(0);
            }

            m_tasks.erase(iterator);
        }
    }

    void TaskContainer::StopAll()
    {
        for (unsigned int i = 0; i < m_tasks.size(); i++)
        {
            // Run update before deleting
            const auto item = m_tasks[i];
            if (item->GetState() != TaskState::Completed)
            {
                item->Stop();
                item->Update(0);
            }

            m_tasks.erase(m_tasks.begin() + i);
        }

        m_tasks.clear();
    }

    void TaskContainer::Update(const double delta)
    {
        for (const auto task : m_tasks)
            task->Update(delta);
    }
}
