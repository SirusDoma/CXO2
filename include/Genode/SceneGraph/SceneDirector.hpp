#ifndef GENODE_SCENEGRAPH_SCENE_DIRECTOR
#define GENODE_SCENEGRAPH_SCENE_DIRECTOR

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>

#include <Genode/Audio/Mixer.hpp>
#include <Genode/Entities.hpp>

#include <typeindex>

namespace Gx
{
    class Scene;
    class Application;
    class SceneDirector : public Renderable, public Updatable, public Inputable
    {
    public:
        SceneDirector(SceneDirector &&director) noexcept;
        SceneDirector(Application &app, sf::RenderTarget &target);
        SceneDirector(Application &app, sf::RenderTarget &target, Scene &scene);
        ~SceneDirector() override;

        RenderStates Render(sf::RenderTarget& target, RenderStates states) const override;
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
        void Present();

        void Unload();

        bool IsCacheEnabled() const;
        void SetCacheEnabled(bool cacheEnabled);

        Application &GetApplication() const;

        template<typename T>
        void Register(std::function<std::unique_ptr<Gx::Scene>()> factory);

    private:
        using SceneCacheMap   = std::unordered_map<std::type_index, std::unique_ptr<Gx::Scene>>;
        using SceneFactoryMap = std::unordered_map<std::type_index, std::function<std::unique_ptr<Gx::Scene>()>>;

        void Stage();
        void Unstage();

        const sf::RenderTarget *m_target;
        Application            *m_application;
        SceneFactoryMap         m_factories;
        SceneCacheMap           m_caches;
        std::unique_ptr<Scene>  m_currentScene, m_nextScene;
        bool                    m_cacheEnabled;
        mutable bool            m_staged;
    };
}

#include <Genode/SceneGraph/SceneDirector.inl>
#endif
