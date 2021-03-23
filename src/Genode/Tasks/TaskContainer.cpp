#include <Genode/Tasks/TaskContainer.hpp>

namespace Gx
{
    TaskContainer::TaskContainer() :
        m_tasks()
    {
    }

    TaskContainer::~TaskContainer()
    {
        StopAll();
    }

    void TaskContainer::Run(Task* task)
    {
        if (task)
            m_tasks.push_back(std::shared_ptr<Task>(task));
    }

    void TaskContainer::Stop(Task* task)
    {
        auto iterator = std::find_if(m_tasks.begin(), m_tasks.end(), [task](auto node) { return task == node.get(); });
        if (iterator != m_tasks.end())
        {
            // Run update before deleting
            auto item = *iterator;
            if (item->GetState() != Task::Completed)
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
            auto item = m_tasks[i];
            if (item->GetState() != Task::Completed)
            {
                item->Stop();
                item->Update(0);
            }

            m_tasks.erase(m_tasks.begin() + i);
        }

        m_tasks.clear();
    }

    void TaskContainer::Update(double delta)
    {
        for (auto task : m_tasks)
            task->Update(delta);
    }
}