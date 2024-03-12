#ifndef GENODE_SCENEGRAPH_SCENE_HPP
#define GENODE_SCENEGRAPH_SCENE_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Window/Event.hpp>

#include <Genode/Audio/Mixer.hpp>
#include <Genode/SceneGraph/Node.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>
#include <Genode/SceneGraph/InputableContainer.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Tasks/TaskContainer.hpp>

#include <queue>
#include <Genode/System/Application.hpp>

namespace Gx
{
    class SceneDirector;
    class Scene : public virtual Node, public virtual RenderableContainer, public virtual UpdatableContainer, public virtual InputableContainer, public virtual TaskContainer
    {
    public:
        friend SceneDirector;

        Scene();
        Scene(const std::string& name);

        virtual ~Scene();

        Application &GetApplication() const;
        SceneDirector &GetDirector() const;
        sf::View GetView() const;

        Node *GetCurrentOverlay() const;
        void PushOverlay(Node *overlay);
        void CloseOverlay();

        void PushEvent(std::function<void()> evt);

    protected:
        virtual void Initialize();
        virtual bool Close(bool quit = false);

        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;
        virtual void Update(double delta);
        virtual bool Input(sf::Event ev);

        virtual void ProcessSceneEvents();

        template<typename T>
        T &Require() const;

    private:
        mutable sf::View m_view;

        SceneDirector     *m_director;
        std::vector<Node*> m_overlays;

        sf::Event m_lastInput;
        std::queue<std::function<void()>> m_events;

        void SetDirector(SceneDirector& director);
        void SetView(const sf::View &view);
    };
}

#include <Genode/SceneGraph/Scene.inl>
#endif
