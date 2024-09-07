#include <Genode/SceneGraph/SceneDirector.hpp>

namespace Gx
{
    template<typename T>
    void SceneDirector::Register(const SceneFactory &factory)
    {
        static_assert(std::is_base_of_v<Scene, T>, "Parameter must be a Gx::Scene");

        m_factories[typeid(T)] = factory;
    }

    template<typename T>
    void SceneDirector::Present(T &scene)
    {
        static_assert(std::is_base_of_v<Scene, T>, "Parameter must be a Gx::Scene.");

        Unstage();

        m_nextScene = std::make_unique<T>(scene);
        m_staged = false;
    }

    template<typename T>
    void SceneDirector::Present(const ResourceContext &context)
    {
        static_assert(std::is_base_of_v<Gx::Scene, T>, "Parameter must be a Gx::Scene.");

        Unstage();

        const auto it = m_factories.find(typeid(T));
        if (it == m_factories.end())
            throw Exception("Scene is not registered");

        // TODO: use cache if enabled
        //       probably need to change the pointer to shared_ptr

        m_nextScene = std::move(it->second(context));
        m_staged = false;
    }


    template<typename T>
    bool SceneDirector::IsPresenting()
    {
        return m_currentScene && dynamic_cast<T*>(m_currentScene.get()) != nullptr;
    }
}