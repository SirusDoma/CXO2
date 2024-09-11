namespace Gx
{
    template<typename... Args>
    void RadioButton::Pair(RadioButton& first, Args... args)
    {
        Pair(first);
        Pair(args...);

        if constexpr (sizeof ...(args) == 0)
            Invalidate();
    }
}