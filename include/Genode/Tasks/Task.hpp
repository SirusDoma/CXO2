#ifndef GENODE_TASKS_TASK_HPP
#define GENODE_TASKS_TASK_HPP

#include <SFML/System/Time.hpp>

#include <Genode/Entities/Updatable.hpp>

#include <functional>

namespace Gx
{
    enum class TaskState
    {
        Initial,
        Running,
        Stopped,
        Completed
    };

    class Task : public Updatable
    {
    public:
        friend class TaskContainer;
        ~Task() override = default;

        TaskState GetState() const;
        virtual const sf::Time &GetElapsed() const;

        void OnStart(std::function<void()> callback);
        void OnStopped(std::function<void()> callback);
        void OnCompleted(std::function<void()> callback);

        virtual void Stop();
        virtual void Complete();
        virtual void Reset();

        void Update(double delta) override;

    protected:
        Task();

        void SetState(const TaskState &state);

    private:
        TaskState m_state;
        sf::Time m_elapsed;

        std::function<void()> m_onStart, m_onStop, m_onComplete;
    };
}

#endif