namespace Gx
{
    template<typename... Args>
    void Control::AddChild(Control* first, Args... args)
    {
        AddChild(first);
        AddChild(args...);

        if constexpr (sizeof ...(args) == 0)
            Invalidate();
    }

    template<typename... Args>
    void Control::RemoveChild(Control* first, Args... args)
    {
        RemoveChild(first);
        RemoveChild(args...);

        if constexpr (sizeof ...(args) == 0)
            Invalidate();
    }
}