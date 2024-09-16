namespace Gx
{
    template<typename... Tasks>
    Sequence::Sequence(Tasks&... tasks) :
        m_tasks{&tasks...},
        m_iterator()
    {
    }

    template<typename... Tasks>
    Sequence::Sequence(const std::function<void()>& callback, Tasks&... tasks) :
        m_tasks{&tasks...},
        m_iterator()
    {
        SetCompletedCallback(callback);
    }

    template<typename... Args>
    Sequence* Sequence::Add(Task& first, Args&... args)
    {
        auto sequence = Add(first);
        return sequence->Add(args...);
    }

    template<typename... Args>
    Sequence* Sequence::Remove(const Task& first, Args&... args)
    {
        auto sequence = Remove(first);
        return sequence->Remove(args...);
    }
}