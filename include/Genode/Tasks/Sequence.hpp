#pragma once

#include <Genode/Tasks/Task.hpp>
#include <Genode/Tasks/Delay.hpp>
#include <vector>

namespace Gx
{
    class Sequence : public Task
    {
    public:
        Sequence();

        template<typename... Tasks>
        explicit Sequence(Tasks&... tasks);

        template<typename... Tasks>
        explicit Sequence(const std::function<void()>& callback,Tasks&... tasks);

        template<typename... Args>
        Sequence* Add(Task& first, Args&... args);
        Sequence* Add(Task& task);

        template<typename... Args>
        Sequence* Remove(const Task& first, Args&... args);
        Sequence* Remove(const Task& task);
        static std::initializer_list<Task&> ListOf(std::initializer_list<Task&> &&tasks);

        void Update(double delta) override;
        void Complete() override;
        void Reset() override;

    private:
        std::vector<Task*> m_tasks;
        std::vector<Task*>::iterator m_iterator;
    };
}

#include <Genode/Tasks/Sequence.inl>
