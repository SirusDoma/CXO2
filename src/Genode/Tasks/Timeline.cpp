#include <Genode/Tasks/Timeline.hpp>
#include <algorithm>

namespace Gx
{
    Timeline::Timeline() : 
        m_frames()
    {
    }

    Timeline::KeyFrame::KeyFrame(const sf::Time& offset, Gx::Task& task) :
        Task(&task),
        Offset(offset)
    {
    }

    bool Timeline::KeyFrame::operator==(const KeyFrame& rhs) const
    {
        return Task == rhs.Task && Offset == rhs.Offset;
    }

    Timeline* Timeline::Add(const KeyFrame& frame)
    {
        if (frame.Task)
            m_frames.push_back(frame);

        return this;
    }
    
    Timeline* Timeline::Remove(const KeyFrame& frame)
    {
        if (const auto iterator = std::find(m_frames.begin(), m_frames.end(), frame); iterator != m_frames.end())
            m_frames.erase(iterator);

        return this;
    }
    
    void Timeline::Update(const double delta)
    {
        Task::Update(delta);

        if (m_frames.size() == 0)
            return Complete();

        auto state = GetState();
        if (state == TaskState::Stopped || state == TaskState::Completed)
            return;

        int completed = 0;
        const sf::Time elapsed = GetElapsed();
        for (const auto frame : m_frames)
        {
            if (frame.Task && frame.Offset <= elapsed)
            {
                state = frame.Task->GetState();
                if (state == TaskState::Initial || state == TaskState::Running)
                    frame.Task->Update(delta);
                else
                    completed++;
            }
        }

        if (completed == m_frames.size())
            Complete();
    }
    
    void Timeline::Reset()
    {
        Task::Reset();
        for (const auto frame : m_frames)
        {
            if (frame.Task)
                frame.Task->Reset();
        }
    }
}
