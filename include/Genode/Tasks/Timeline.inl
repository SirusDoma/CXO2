namespace Gx
{
    template<typename... Args>
    Timeline* Timeline::Add(KeyFrame* first, Args... args)
    {
        auto sequence = Add(first);
        return sequence->Add(args...);
    }

    template<typename... Args>
    Timeline* Timeline::Remove(KeyFrame* first, Args... args)
    {
        auto sequence = Remove(first);
        return sequence->Remove(args...);
    }
}