
namespace Gx
{
    template<typename T>
    T &Scene::Require() const
    {
        return Application::Instance().Require<T>();
    }
}