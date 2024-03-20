#ifndef GENODE_TASKS_TASK_HPP
#define GENODE_TASKS_TASK_HPP

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

        ~Task() override = default;

        const TaskState GetState() const;
        virtual const sf::Time GetElapsed() const;

        void OnStart(std::function<void()> callback);
        void OnStopped(std::function<void()> callback);
        void OnCompleted(std::function<void()> callback);

        void Update(double delta) override;
        virtual void Reset();

    protected:
        Task();

        void SetState(const TaskState &state);

        virtual void Stop();
        virtual void Complete();

    private:
        TaskState m_state;
        sf::Time m_elapsed;

        std::function<void()> m_onStart, m_onStop, m_onComplete;
    };
}

#endif