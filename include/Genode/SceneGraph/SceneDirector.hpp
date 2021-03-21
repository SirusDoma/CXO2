#ifndef GENODE_SCENEGRAPH_DIRECTOR
#define GENODE_SCENEGRAPH_DIRECTOR

#include <Genode/Entities.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>

namespace Gx
{
    class Scene;
    class SceneDirector : public Renderable, public Updatable, public Inputable
    {
    public:
        friend class Application;

        explicit SceneDirector(Scene* scene, sf::RenderTarget *target);
        ~SceneDirector();

        Scene* GetScene() const;
        void SetScene(Scene* scene);

    private:
        std::unique_ptr<Scene> m_scene;
        sf::RenderTarget *m_target;

        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual void Update(double delta);
        virtual bool Input(sf::Event ev);
        virtual void ProcessEvents();

        bool Close();
    };
}

#endif
