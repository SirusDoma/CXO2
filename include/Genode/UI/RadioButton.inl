namespace Gx
{
    template<typename... Args>
    void RadioButton::Pair(RadioButton *first, Args... args)
    {
        Pair(first);
        Pair(args...);
    }
}