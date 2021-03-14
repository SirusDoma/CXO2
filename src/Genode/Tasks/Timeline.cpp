#include <Genode/Tasks/Timeline.hpp>
#include <algorithm>

namespace Gx
{
    Timeline::Timeline() : 
        m_frames()
    {
    }
    
    Timeline::~Timeline()
    {
        for (auto frame : m_frames)
        {
            if (frame) 
                delete frame;
        }

        m_frames.clear();
    }

    Timeline::KeyFrame::KeyFrame(const sf::Time& offset, Gx::Task* task) :
        Offset(offset),
        KeyTask(task)
    {
    }

    Timeline::KeyFrame::~KeyFrame()
    {
        if (KeyTask)
            delete KeyTask;
    }
    
    Timeline* Timeline::Add(KeyFrame* frame)
    {
        if (frame)
            m_frames.push_back(frame);

        return this;
    }
    
    Timeline* Timeline::Remove(KeyFrame* frame)
    {
        if (frame)
        {
            auto iterator = std::find(m_frames.begin(), m_frames.end(), frame);
            if (iterator != m_frames.end())
            {
                delete* iterator;
                m_frames.erase(iterator);
            }
        }

        return this;
    }
    
    void Timeline::Update(double delta)
    {
        Task::Update(delta);

        if (m_frames.size() == 0)
            return Complete();

        auto state = GetState();
        if (state == TaskState::Stopped || state == TaskState::Completed)
            return;

        int completed = 0;
        sf::Time elapsed = GetElapsed();
        for (auto frame : m_frames)
        {
            if (frame->KeyTask && frame->Offset <= elapsed)
            {
                state = frame->KeyTask->GetState();
                if (state == TaskState::Initial || state == TaskState::Running)
                    frame->KeyTask->Update(delta);
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
        for (auto frame : m_frames)
        {
            if (frame->KeyTask)
                frame->KeyTask->Reset();
        }
    }
}