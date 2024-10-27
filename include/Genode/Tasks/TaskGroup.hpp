#pragma once

#include <Genode/Tasks/Task.hpp>

namespace Gx
{
    class TaskGroup : public Task
    {
    public:
        TaskGroup() = default;

        template<typename... Tasks>
        explicit TaskGroup(Tasks&... tasks);

        template<typename... Tasks>
        explicit TaskGroup(const std::function<void()>& callback, Tasks&... tasks);

        template<typename... Args>
        TaskGroup* Add(Task& first, Args&... args);
        TaskGroup* Add(Task& task);

        template<typename... Args>
        TaskGroup* Remove(const Task& first, Args&... args);
        TaskGroup* Remove(const Task& task);

    protected:
        void Update(double delta) override;
        void Stop() override;
        void Complete() override;
        void Reset() override;

    private:
        std::function<void()> m_callback;
        std::vector<Task*> m_tasks;
    };
}

#include <Genode/Tasks/TaskGroup.inl>