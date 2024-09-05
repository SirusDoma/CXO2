namespace Gx
{
    template<typename T>
    T& Scene::Require() const
    {
        return GetContext().Require<T>();
    }
}