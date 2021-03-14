namespace Gx
{
    template<typename... Args>
    void Animation::AddFrame(sf::IntRect first, Args... args)
    {
        AddFrame(first);
        AddFrame(args...);
    }
}
