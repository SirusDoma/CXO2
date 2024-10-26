#pragma once

#include <Genode/System/Context.hpp>

#include <Genode/Audio/Mixer.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>
#include <Genode/SceneGraph/InputableContainer.hpp>

#include <Genode/Tasks/TaskContainer.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include <queue>

namespace Gx
{
    class Application;
    class SceneDirector;
    class Scene : public virtual Node, public virtual RenderableContainer, public virtual UpdatableContainer, public virtual InputableContainer, public virtual TaskContainer
    {
    public:
        friend SceneDirector;

        Scene();
        explicit Scene(const std::string& name);

        ~Scene() override;

        Application& GetApplication() const;
        Context& GetContext() const;
        SceneDirector& GetDirector() const;

        sf::View GetView() const;
        sf::View GetInitialView() const;
        sf::View GetDefaultView() const;

        Node* GetCurrentOverlay() const;
        void PushOverlay(Node& overlay);
        void CloseOverlay();

        template<typename T>
        T& Require() const;

        void QueueEvent(const std::function<void()>& evt);

    protected:
        void Initialize() override;
        virtual bool Close(bool quit);

        RenderStates Render(RenderSurface& surface, RenderStates states) const override;
        void Update(double delta) override;
        bool Input(const sf::Event& ev) override;

        virtual void ProcessEvents();

    private:
        bool IsVisible() const override { return true; }
        void SetVisible(const bool visible) override {}

        mutable sf::View m_view;

        SceneDirector*     m_director;
        std::vector<Node*> m_overlays;

        std::optional<sf::Event> m_lastInput;
        std::queue<std::function<void()>> m_events;

        void SetDirector(SceneDirector& director);
        void SetView(const sf::View& view);
    };
}

#include <Genode/SceneGraph/Scene.inl>
