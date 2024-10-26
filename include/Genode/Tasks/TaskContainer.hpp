#pragma once

#include <Genode/Entities/Updatable.hpp>
#include <Genode/Tasks/Task.hpp>

#include <vector>

namespace Gx
{
    class TaskContainer : public virtual Updatable
    {
    public:
        TaskContainer();

        void Run(Task& task);
        void Stop(const Task& task);

        template<typename... Args>
        void Run(Task& first, Args&... args);

        template<typename... Args>
        void Stop(const Task& first, const Args&... args);

        void StopAll();

    protected:
        void Update(double delta) override;

    private:
        std::vector<Task*> m_tasks;
    };
}

#include <Genode/Tasks/TaskContainer.inl>
