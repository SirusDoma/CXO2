#include <Genode/Tasks/Sequence.hpp>
#include <algorithm>

namespace Gx
{
    void Sequence::Initialize()
    {
        Task::Initialize();

        m_iterator = m_tasks.begin();
    }

    void Sequence::Update(const double delta)
    {
        Task::Update(delta);
        if (m_tasks.size() == 0)
            return Complete();

        if (GetState() != TaskState::Running)
            return;

        if (m_iterator != m_tasks.end())
        {
            const auto task = m_iterator->get();
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
        if (GetState() == TaskState::Completed)
            return;

        for (const auto& task : m_tasks)
            task->Complete();

        Task::Complete();
    }

    void Sequence::Reset()
    {
        if (GetState() == TaskState::Idle)
            return;

        for (const auto& task : m_tasks)
            task->Reset();

        m_iterator = m_tasks.begin();
        Task::Reset();
    }
}
