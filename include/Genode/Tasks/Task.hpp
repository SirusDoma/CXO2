#ifndef GENODE_TASK_HPP
#define GENODE_TASK_HPP

#include <SFML/System/Time.hpp>

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

        const TaskState GetState() const;
        virtual const sf::Time GetElapsed() const;

        void OnStart(std::function<void()> callback);
        void OnStopped(std::function<void()> callback);
        void OnCompleted(std::function<void()> callback);

        virtual void Update(double delta);
        virtual void Reset();

    protected:
        Task();

        void SetState(const TaskState &state);

        virtual void Stop();
        virtual void Complete();

    private:
        TaskState m_state;
        sf::Time m_elapsed;

        std::function<void()> m_start, m_stop, m_complete;
    };
}

#endif