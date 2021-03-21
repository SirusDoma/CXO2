#ifndef GENODE_SCENE_HPP
#define GENODE_SCENE_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Window/Event.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>
#include <Genode/SceneGraph/InputableContainer.hpp>
#include <Genode/Entities.hpp>
#include <Genode/Tasks/TaskContainer.hpp>

#include <queue>

namespace Gx
{
    class SceneDirector;
    class Scene : public virtual Node, public RenderableContainer, public virtual UpdatableContainer, public InputableContainer, public TaskContainer
    {
    public:
        friend SceneDirector;

        Scene();
        Scene(const std::string& name);
        virtual ~Scene();

        SceneDirector* GetDirector() const;
        sf::View GetView() const;

        void PushOverlay(Node *overlay);
        void CloseOverlay();

        void QueueEvent(std::function<void()> evt);

    protected:
        virtual void Initialize();
        virtual bool Close(bool quit = false);

        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;
        virtual void Update(double delta);
        virtual bool Input(sf::Event ev);

        virtual void ProcessEvents();

    private:
        mutable sf::View m_view;
        SceneDirector *m_director;
        std::vector<Node*> m_overlays;
        std::queue<std::function<void()>> m_events;

        void SetDirector(SceneDirector* director);
        void SetView(const sf::View &view);
    };
}

#endif
