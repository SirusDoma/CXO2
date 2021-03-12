#ifndef GENODE_TASK_HPP
#define GENODE_TASK_HPP

#include <Genode/Entities/Updatable.hpp>
#include <functional>

namespace Gx
{
    class Task : public Updatable
    {
    public:
        friend class TaskContainer;

        enum TaskState
        {
            Initial,
            Running,
            Stopped,
            Completed
        };

        virtual ~Task();

        TaskState GetState() const;
        double GetElapsed() const;

        void OnStart(void(*callback)());
        void OnStopped(void(*callback)());
        void OnCompleted(void(*callback)());

        virtual void Update(double delta);
        virtual void Reset();

    protected:
        Task();

        virtual void Stop();
        virtual void Complete();

    private:
        TaskState m_state;
        double m_elapsed;

        std::function<void()> m_start, m_stop, m_complete;
    };
}

#endif