namespace Gx
{
    template<typename... Tasks>
    TaskGroup::TaskGroup(Tasks&... tasks) :
        m_tasks{&tasks...}
    {
    }

    template<typename... Tasks>
    TaskGroup::TaskGroup(const std::function<void()>& callback, Tasks&... tasks) :
        m_tasks{&tasks...}
    {
        SetCompletedCallback(callback);
    }

    template<typename... Args>
    TaskGroup* TaskGroup::Add(Task& first, Args&... args)
    {
        auto group = Add(first);
        return group->Add(args...);
    }

    template<typename... Args>
    TaskGroup* TaskGroup::Remove(const Task& first, Args&... args)
    {
        auto group = Remove(first);
        return group->Remove(args...);
    }
}