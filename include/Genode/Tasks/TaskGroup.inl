#pragma once

namespace Gx
{
    template<typename... Tasks, std::enable_if_t<std::conjunction_v<std::is_base_of<Task, std::decay_t<Tasks>>...>, int>>
    TaskGroup::TaskGroup(Tasks&&... tasks)
    {
        Add(std::forward<Tasks>(tasks)...);
    }

    template<typename... Tasks, std::enable_if_t<std::conjunction_v<std::is_base_of<Task, std::decay_t<Tasks>>...>, int>>
    TaskGroup::TaskGroup(const std::function<void()>& callback, Tasks&&... tasks) :
        TaskGroup(std::forward<Tasks>(tasks)...)
    {
        SetCompletedCallback(callback);
    }

    template<typename T, typename ... Args>
    std::enable_if_t<std::is_base_of_v<Task, T>, TaskGroup&>
    TaskGroup::Add(Args&&... args)
    {
        auto item = std::make_shared<T>(std::forward<Args>(args)...);
        auto ref  = std::weak_ptr<T>(item);

        m_tasks.push_back(std::move(item));
        return ref;
    }

    template<typename... Tasks>
    std::enable_if_t<std::conjunction_v<std::is_base_of<Task, std::decay_t<Tasks>>...>, TaskGroup&>
    TaskGroup::Add(Tasks&&... tasks)
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
        return *this;
    }
}