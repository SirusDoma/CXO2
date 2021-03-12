#include <Genode/Tasks/TaskContainer.hpp>

namespace Gx
{
    TaskContainer::TaskContainer() :
        m_tasks()
    {
    }

    TaskContainer::~TaskContainer()
    {
        for (auto task : m_tasks)
            delete task;

        m_tasks.clear();
    }

    void TaskContainer::Run(Task* task)
    {
        if (task)
            m_tasks.push_back(task);
    }

    void TaskContainer::Stop(Task* task)
    {
        auto iterator = std::find(m_tasks.begin(), m_tasks.end(), task);
        if (iterator != m_tasks.end())
        {
            // Run update before deleting
            auto item = *iterator;
            item->Stop();
            item->Update(0);

            delete *iterator;
            m_tasks.erase(iterator);
        }
    }

    void TaskContainer::StopAll()
    {
        for (unsigned int i = 0; i < m_tasks.size(); i++)
        {
            // Run update before deleting
            auto item = m_tasks[i];
            item->Stop();
            item->Update(0);

            delete item;
            m_tasks.erase(m_tasks.begin() + i);
        }
    }

    void TaskContainer::Update(double delta)
    {
        for (unsigned int i = 0; i < m_tasks.size(); i++)
        {
            auto task = m_tasks[i];
            if (task)
            {
                if (task->GetState() == Task::Completed)
                {
                    delete task;
                    m_tasks.erase(m_tasks.begin() + i);
                }
                else
                    task->Update(delta);
            }
            else
                m_tasks.erase(m_tasks.begin() + i);
        }
    }
}