#pragma once

namespace Gx
{
    template<typename T, typename ... Args>
    std::enable_if_t<std::is_base_of_v<Task, T>, std::weak_ptr<T>>
    TaskContainer::Run(Args&&... args)
    {
        auto item = std::make_shared<T>(std::forward<Args>(args)...);
        auto ref  = std::weak_ptr<T>(item);

        m_tasks.push_back(std::move(item));
        return ref;
    }

    template<typename T>
    std::enable_if_t<std::is_base_of_v<Task, T>, T&>
    TaskContainer::Run(T& task)
    {
        const auto it = std::find_if(m_tasks.begin(), m_tasks.end(), [&](const auto& t)
        {
            return t.get() == &task;
        });

        if (it == m_tasks.end())
        {
            auto item = std::shared_ptr<Task>(&task, [] (auto _) {});
            m_tasks.push_back(std::move(item));
        }
        else if ((*it)->GetState() != TaskState::Idle)
            (*it)->Reset();

        return task;
    }

    template<typename... Tasks>
    auto TaskContainer::Run(Tasks&&... tasks) ->
        std::enable_if_t<std::conjunction_v<
            std::is_base_of<Task, std::decay_t<Tasks>>...
        >, std::tuple<std::weak_ptr<std::decay_t<Tasks>>...>>
    {
        auto construct = [this] (auto&& task)
        {
            if constexpr (std::is_rvalue_reference_v<decltype(task)>)
            {
                static_assert(std::is_copy_constructible_v<std::decay_t<decltype(task)>>, "Specified r-value Task must be a copy constructible type.");

                return std::static_pointer_cast<std::decay_t<decltype(task)>>(
                    m_tasks.emplace_back(std::make_shared<std::decay_t<decltype(task)>>(std::forward<decltype(task)>(task)))
                );
            }
            else
            {
                return std::static_pointer_cast<std::decay_t<decltype(task)>>(
                   m_tasks.emplace_back(std::shared_ptr<std::decay_t<decltype(task)>>(&task, [](std::decay_t<decltype(task)>*) {}))
                );
            }
        };

        return std::make_tuple(
            std::weak_ptr<std::decay_t<Tasks>>(construct(std::forward<Tasks>(tasks)))...
        );
    }

    template<typename... Tasks>
    std::enable_if_t<std::conjunction_v<std::is_base_of<Task, Tasks>...>, void>
    TaskContainer::Stop(const Tasks&... tasks)
    {
        (Stop(static_cast<const Task&>(tasks)), ...);
    }
}