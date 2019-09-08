namespace Gx
{
    template<typename T>
    Sequence* Sequence::Routine(T* task, unsigned int count, double delay)
    {
        static_assert(std::is_base_of<Task, T>::value, "Parameter must be a Gx::Task");

        auto sequence = new Sequence();
        for (unsigned int i = 0; i < count; i++)
            sequence->Add(new Delay(delay), new T(*task));

        return sequence;
    }

    template<typename... Args>
    Sequence* Sequence::Add(Task* first, Args... args)
    {
        auto sequence = Add(first);
        return sequence->Add(args...);
    }

    template<typename... Args>
    Sequence* Sequence::Remove(Task* first, Args... args)
    {
        auto sequence = Remove(first);
        return sequence->Remove(args...);
    }
}