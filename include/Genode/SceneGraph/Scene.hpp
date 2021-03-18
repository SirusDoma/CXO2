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

        void SetOverlay(Node *overlay);
        Node *GetOverlay() const;
        void CloseOverlay();

    protected:
        virtual void Initialize();
        virtual bool Close(bool quit = false);

        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;
        virtual void Update(double delta);
        virtual bool Input(sf::Event ev);

    private:
        SceneDirector *m_director;
        Node *m_overlay;

        mutable sf::View m_view;
        mutable std::queue<std::function<void()>> m_deferrables;

        void SetDirector(SceneDirector* director);
        void SetView(const sf::View &view);

        void ProcessDeferrables() const;
    };
}

#endif
