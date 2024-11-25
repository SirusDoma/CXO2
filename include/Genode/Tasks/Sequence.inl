#pragma once

namespace Gx
{
    template<typename... Tasks, std::enable_if_t<std::conjunction_v<std::is_base_of<Task, std::decay_t<Tasks>>...>, int>>
    Sequence::Sequence(Tasks&&... tasks)
    {
        auto insert = [this] (auto&& task)
        {
            using TaskType = std::decay_t<decltype(task)>;
            if constexpr (std::is_rvalue_reference_v<decltype(task)>)
            {
                m_tasks.emplace_back(
                    ResourcePtr<Task>(new TaskType(std::move(task)), [](auto ptr) { delete ptr; })
                );
            }
            else
            {
                m_tasks.emplace_back(
                    ResourcePtr<Task>(&task, [](auto) {})
                );
            }
        };

        (insert(std::forward<Tasks>(tasks)), ...);
        m_iterator = m_tasks.begin();
    }

    template<typename... Tasks, std::enable_if_t<std::conjunction_v<std::is_base_of<Task, std::decay_t<Tasks>>...>, int>>
    Sequence::Sequence(const std::function<void()>& callback, Tasks&&... tasks) :
        Sequence(std::forward<Tasks>(tasks)...)
    {
        SetCompletedCallback(callback);
    }
}
