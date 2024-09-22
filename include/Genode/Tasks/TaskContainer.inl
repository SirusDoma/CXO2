namespace Gx
{
    template<typename... Args>
    void TaskContainer::Run(Task& first, Args&... args)
    {
        Run(first);
        Run(args...);
    }

    template<typename... Args>
    void TaskContainer::Stop(const Task& first, const Args&... args)
    {
        Stop(first);
        Stop(args...);
    }
}