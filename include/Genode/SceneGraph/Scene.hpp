#pragma once

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>
#include <Genode/SceneGraph/InputableContainer.hpp>
#include <Genode/SceneGraph/TaskContainer.hpp>

#include <Genode/Entities/Presentable.hpp>

#include <Genode/System/Context.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include <queue>
#include <set>

namespace Gx
{
    class Application;
    class Scene : public virtual Node, public RenderableContainer, public UpdatableContainer,
                  public InputableContainer, public TaskContainer, public Presentable::Parent
    {
    public:
        friend class SceneDirector;

        ~Scene() override = default;

        static bool IsTrackable();

        Application& GetApplication() const;
        SceneDirector& GetDirector() const;
        Context& GetContext();

        const sf::View& GetView() const;
        const sf::View& GetDefaultView() const;
        void SetView(const sf::View& view) const;

        bool IsPresenting(Presentable& presentable) const override;
        void Present(Presentable& presentable, const PresentationContext& context = PresentationContext::Default) override;
        bool Dismiss(Presentable& presentable) override;
        bool Dismiss() override;

        template<typename T>
        T& Require();

        void QueueEvent(const std::function<void()>& evt);

    protected:
        Scene();
        explicit Scene(const std::string& name);

        void Initialize() override;
        void Finalize() override;

        virtual void OnAppFocusChanged(bool focus);
        virtual void OnAppResized(const sf::Vector2u& size);
        virtual bool OnAppClose();

        RenderStates Render(RenderSurface& surface, RenderStates states) const override;
        void Update(double delta) override;
        bool Input(const sf::Event& ev) override;

    private:
        bool IsVisible() const override { return true; }
        void SetVisible(const bool visible) override {}

        void SetDirector(SceneDirector& director);
        void SetContext(Context&& context);

        void ProcessEvents();

        mutable sf::View m_view{};

        SceneDirector*         m_director{nullptr};
        std::set<Presentable*> m_presentables;

        std::optional<sf::Event> m_lastInput{};
        std::queue<std::function<void()>> m_events{};

        bool m_initialized{};
        std::optional<Context> m_context;

    };
}

#include <Genode/SceneGraph/Scene.inl>
