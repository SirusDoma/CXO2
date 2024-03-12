#ifndef GENODE_TASKS_TASK_CONTAINER_HPP
#define GENODE_TASKS_TASK_CONTAINER_HPP

#include <memory>
#include <Genode/Entities/Updatable.hpp>
#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    class TaskContainer : public virtual Updatable
    {
    public:
        TaskContainer();
        virtual ~TaskContainer();

        void Run(Task* task);
        void Stop(Task* task);

        template<typename... Args>
        void Run(Task* first, Args... args);

        template<typename... Args>
        void Stop(Task* first, Args... args);

        void StopAll();

    protected:
        virtual void Update(double delta);

    private:
        std::vector<Task*> m_tasks;
    };
}

#include <Genode/Tasks/TaskContainer.inl>
#endif