#pragma once

#include <Genode/Entities/Updatable.hpp>
#include <Genode/Tasks/Task.hpp>

#include <vector>
#include <memory>

namespace Gx
{
    class TaskContainer : public virtual Updatable
    {
    public:
        TaskContainer();
        TaskContainer(const TaskContainer& other) = delete;

        TaskContainer& operator=(const TaskContainer& other) = delete;

        template<typename T, typename... Args>
        std::enable_if_t<std::is_base_of_v<Task, T>, std::weak_ptr<T>>
        Run(Args&&... args);

        template<typename T>
        std::enable_if_t<std::is_base_of_v<Task, T>, T&>
        Run(T& task);

        template<typename... Tasks>
        auto Run(Tasks&&... tasks) ->
            std::enable_if_t<std::conjunction_v<
                std::is_base_of<Task, std::decay_t<Tasks>>...
            >, std::tuple<std::weak_ptr<std::decay_t<Tasks>>...>>;

        void Stop(const Task& task);

        template<typename... Tasks>
        std::enable_if_t<std::conjunction_v<std::is_base_of<Task, Tasks>...>, void>
        Stop(const Tasks&... tasks);

        void StopAll();

    protected:
        void Update(double delta) override;

    private:
        TaskState m_state;
        std::vector<std::shared_ptr<Task>> m_tasks;
    };
}

#include <Genode/SceneGraph/TaskContainer.inl>
