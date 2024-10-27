#include <Genode/Tasks/Sequence.hpp>
#include <algorithm>

namespace Gx
{
    Sequence::Sequence() :
        m_tasks(),
        m_iterator()
    {
    }

    Sequence* Sequence::Add(Task& task)
    {
        m_tasks.push_back(&task);
        return this;
    }

    Sequence* Sequence::Remove(const Task& task)
    {
        const auto iterator = std::find(m_tasks.begin(), m_tasks.end(), &task);
        if (iterator != m_tasks.end())
            m_tasks.erase(iterator);

        return this;
    }

    void Sequence::Update(const double delta)
    {
        Task::Update(delta);
        if (m_tasks.size() == 0)
            return Complete();

        if (GetState() == TaskState::Initial)
            m_iterator = m_tasks.begin();
        else if (GetState() != TaskState::Running)
            return;

        if (m_iterator != m_tasks.end())
        {
            const auto task = *m_iterator;
            task->Update(delta);
            if (task->GetState() == TaskState::Completed || task->GetState() == TaskState::Stopped)
                ++m_iterator;
        }
        else
        {
            m_iterator = m_tasks.end();
            return Complete();
        }
    }

    void Sequence::Complete()
    {
        Task::Complete();

        for (const auto task : m_tasks)
            task->Complete();
    }

    void Sequence::Reset()
    {
        Task::Reset();

        m_iterator = m_tasks.begin();
        for (const auto task : m_tasks)
            task->Reset();
    }
}
