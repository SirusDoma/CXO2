
namespace Gx
{
    template<typename T>
    T &Scene::Require() const
    {
        return GetApplication().Require<T>();
    }
}