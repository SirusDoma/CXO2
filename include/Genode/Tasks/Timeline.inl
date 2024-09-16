namespace Gx
{
    template<typename... Args>
    Timeline* Timeline::Add(const KeyFrame& first, const Args&... args)
    {
        auto sequence = Add(first);
        return sequence->Add(args...);
    }

    template<typename... Args>
    Timeline* Timeline::Remove(const KeyFrame& first, const Args&... args)
    {
        auto sequence = Remove(first);
        return sequence->Remove(args...);
    }
}