#pragma once

#include <Genode/Tasks/Task.hpp>
#include <Genode/IO/Resource.hpp>

#include <memory>
#include <functional>

namespace Gx
{
    class TaskGroup : public Task
    {
    public:
        TaskGroup() = default;

        template<typename... Tasks, std::enable_if_t<std::conjunction_v<std::is_base_of<Task, std::decay_t<Tasks>>...>, int> = 0>
        explicit TaskGroup(Tasks&&... tasks);

        template<typename... Tasks, std::enable_if_t<std::conjunction_v<std::is_base_of<Task, std::decay_t<Tasks>>...>, int> = 1>
        explicit TaskGroup(const std::function<void()>& callback, Tasks&&... tasks);

        template<typename T, typename ... Args>
        std::enable_if_t<std::is_base_of_v<Task, T>, TaskGroup&>
        Add(Args&&... args);

        template<typename... Tasks>
        std::enable_if_t<std::conjunction_v<std::is_base_of<Task, std::decay_t<Tasks>>...>, TaskGroup&>
        Add(Tasks&&... tasks);

        TaskGroup& Add(Task& task);
        TaskGroup& Remove(const Task& task);

    protected:
        void Update(double delta) override;
        void Stop() override;
        void Complete() override;
        void Reset() override;

    private:
        std::function<void()> m_callback;
        std::vector<ResourcePtr<Task>> m_tasks;
    };
}

#include <Genode/Tasks/TaskGroup.inl>