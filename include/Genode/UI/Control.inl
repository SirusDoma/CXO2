namespace Gx
{
    template<typename... Args>
    void Control::AddChild(Control& first, Args&... args)
    {
        Control::AddChild(first);
        Control::AddChild(args...);

        if constexpr (sizeof ...(args) == 0)
            Invalidate();
    }

    template<typename... Args>
    void Control::RemoveChild(Control& first, Args&... args)
    {
        Control::RemoveChild(first);
        Control::RemoveChild(args...);

        if constexpr (sizeof ...(args) == 0)
            Invalidate();
    }
}