#ifndef GENODE_SCENEGRAPH_SCENE_DIRECTOR
#define GENODE_SCENEGRAPH_SCENE_DIRECTOR

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>

#include <Genode/Audio/Mixer.hpp>
#include <Genode/Entities.hpp>
#include <Genode/IO/ResourceContext.hpp>

#include <typeindex>

namespace Gx
{
    class Scene;
    class Application;
    class SceneDirector : public Renderable, public Updatable, public Inputable
    {
    public:
        using SceneFactory = std::function<ResourcePtr<Scene>(const ResourceContext&)>;

        SceneDirector(SceneDirector &&director) noexcept;
        explicit SceneDirector(Application &app);
        SceneDirector(Application &app, Scene &scene);
        ~SceneDirector() override;

        RenderStates Render(RenderSurface &surface, RenderStates states) const override;
        void Update(double delta) override;
        bool Input(sf::Event ev) override;

        virtual void ProcessEvents();
        bool Close();

        Scene &GetPresentedScene() const;

        template<typename T>
        bool IsPresenting();

        template<typename T>
        void Present(T &scene);

        template<typename T>
        void Present(const ResourceContext &context = Gx::ResourceContext(typeid(T).name()));

        void Unload();

        bool IsCacheEnabled() const;
        void SetCacheEnabled(bool cacheEnabled);

        Application &GetApplication() const;

        template<typename T>
        void Register(const SceneFactory &factory);

    private:
        using SceneCacheMap   = std::unordered_map<std::type_index, ResourcePtr<Scene>>;
        using SceneFactoryMap = std::unordered_map<std::type_index, SceneFactory>;

        void Stage();
        void Unstage();

        Application       *m_application;
        SceneFactoryMap    m_factories;
        SceneCacheMap      m_caches;
        ResourcePtr<Scene> m_currentScene, m_nextScene;
        bool               m_cacheEnabled;
        mutable bool       m_staged;
    };
}

#include <Genode/SceneGraph/SceneDirector.inl>
#endif
