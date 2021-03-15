#include <Genode/Tasks/Sequence.hpp>
#include <algorithm>

namespace Gx
{
    Sequence::Sequence() :
        m_iterator(),
        m_tasks()
    {
    }

    Sequence::Sequence(std::initializer_list<Task*> tasks) :
        m_iterator(),
        m_tasks(tasks)
    {
    }

    Sequence::Sequence(std::function<void()> callback, std::initializer_list<Task *> tasks) :
        m_iterator(),
        m_tasks(tasks)
    {
        OnCompleted(callback);
    }

    Sequence::~Sequence()
    {
        for (auto task : m_tasks)
            delete task;

        m_tasks.clear();
    }

    Sequence* Sequence::Add(Task* task)
    {
        if (task)
            m_tasks.push_back(task);

        return this;
    }

    Sequence* Sequence::Remove(Task* task)
    {
        if (task)
        {
            auto iterator = std::find(m_tasks.begin(), m_tasks.end(), task);
            if (iterator != m_tasks.end())
            {
                delete* iterator;
                m_tasks.erase(iterator);
            }
        }

        return this;
    }

    void Sequence::Update(double delta)
    {
        Task::Update(delta);
        if (m_tasks.size() == 0)
            return Complete();

        if (GetState() == TaskState::Initial)
            m_iterator = m_tasks.begin();
        else if (GetState() != TaskState::Running)
            return;
        else if (m_iterator == m_tasks.end())
            return Complete();

        auto task = *m_iterator;
        if (task)
        {
            task->Update(delta);
            if (task->GetState() == TaskState::Completed || task->GetState() == TaskState::Stopped)
                ++m_iterator;
        }
        else
            ++m_iterator;
    }

    void Sequence::Reset()
    {
        Task::Reset();

        m_iterator = m_tasks.begin();
        for (auto task : m_tasks)
            task->Reset();
    }
}