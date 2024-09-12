namespace Gx
{
    template<typename... Args>
    void Animation::AddFrame(const Frame& first, Args... args)
    {
        AddFrame(first);
        AddFrame(args...);
    }
}
