#pragma once

#include <Genode/Tasks/Task.hpp>
#include <Genode/IO/Resource.hpp>

#include <vector>
#include <memory>

namespace Gx
{
    class Sequence : public Task
    {
    public:
        template<typename... Tasks, std::enable_if_t<std::conjunction_v<std::is_base_of<Task, std::decay_t<Tasks>>...>, int> = 0>
        explicit Sequence(Tasks&&... tasks);

        template<typename... Tasks, std::enable_if_t<std::conjunction_v<std::is_base_of<Task, std::decay_t<Tasks>>...>, int> = 1>
        explicit Sequence(const std::function<void()>& callback, Tasks&&... tasks);

        void Update(double delta) override;
        void Complete() override;
        void Reset() override;

    protected:
        void Initialize() override;

    private:
        std::vector<ResourcePtr<Task>> m_tasks;
        std::vector<ResourcePtr<Task>>::iterator m_iterator;
    };
}

#include <Genode/Tasks/Sequence.inl>
