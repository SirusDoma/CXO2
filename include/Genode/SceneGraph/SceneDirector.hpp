#ifndef GENODE_SCENEGRAPH_SCENEGRAPH_DIRECTOR
#define GENODE_SCENEGRAPH_SCENEGRAPH_DIRECTOR

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>

#include <Genode/Audio/Mixer.hpp>
#include <Genode/Entities.hpp>
#include <Genode/IO/ResourceManager.hpp>

namespace Gx
{
    class Scene;
    class SceneDirector : public Renderable, public Updatable, public Inputable
    {
    public:
        friend class Application;

        SceneDirector(Application &app, Scene *scene, const sf::RenderTarget &target);
        ~SceneDirector();

        Scene* GetScene() const;
        void SetScene(Scene* scene);

        Application &GetApplication() const;
        ResourceManager &GetSharedResources() const;

    private:
        const sf::RenderTarget  *m_target;
        Application             *m_application;
        ResourceManager         *m_resources;
        std::unique_ptr<Scene>   m_scene;

        void Initialize();
        void SetSharedResources(ResourceManager &resources);

        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual void Update(double delta);
        virtual bool Input(sf::Event ev);
        virtual void ProcessEvents();

        bool Close();
    };
}

#endif
