#ifndef GENODE_TASKS_TASK_CONTAINER_HPP
#define GENODE_TASKS_TASK_CONTAINER_HPP

#include <Genode/Entities/Updatable.hpp>
#include <Genode/Tasks/Task.hpp>

#include <vector>

namespace Gx
{
    class TaskContainer : public virtual Updatable
    {
    public:
        TaskContainer();
        ~TaskContainer() override = default;

        void Run(Task* task);
        void Stop(Task* task);

        template<typename... Args>
        void Run(Task* first, Args... args);

        template<typename... Args>
        void Stop(Task* first, Args... args);

        void StopAll();

    protected:
        void Update(double delta) override;

    private:
        std::vector<Task*> m_tasks;
    };
}

#include <Genode/Tasks/TaskContainer.inl>
#endif