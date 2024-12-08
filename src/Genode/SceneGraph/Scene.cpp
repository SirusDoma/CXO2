#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/System/Application.hpp>

namespace Gx
{
    Scene::Scene() :
        Scene::Scene(Gx::StringHelper::GetTypeName(typeid(this)))
    {
    }

    Scene::Scene(const std::string& name)
    {
        SetName(name);
        SetTag("Scene");
    }

    bool Scene::IsTrackable()
    {
        return true;
    }

    void Scene::Initialize()
    {
        Node::Initialize();
    }

    void Scene::Finalize()
    {
        Node::Finalize();
    }

    void Scene::OnAppFocusChanged(bool focus)
    {
    }

    void Scene::OnAppResized(const sf::Vector2u& size)
    {
    }

    bool Scene::OnAppClose()
    {
        return true;
    }

    Application& Scene::GetApplication() const
    {
        if (!m_director)
            return Application::Instance();

        return m_director->GetApplication();
    }

    Context& Scene::GetContext()
    {
        if (m_context.has_value())
            return m_context.value();

        return GetDirector().GetContext();
    }

    SceneDirector& Scene::GetDirector() const
    {
        if (!m_director)
            throw InvalidOperationException("SceneDirector is not ready yet");

        return *m_director;
    }

    void Scene::SetDirector(SceneDirector& director)
    {
        m_director = &director;
    }

    void Scene::SetContext(Context&& context)
    {
        m_context = std::move(context);
    }

    const sf::View& Scene::GetView() const
    {
        return GetDirector().GetView();
    }

    const sf::View& Scene::GetDefaultView() const
    {
        return GetDirector().GetDefaultView();
    }

    void Scene::SetView(const sf::View& view) const
    {
        GetDirector().SetView(view);
    }

    bool Scene::IsPresenting(Presentable& presentable) const
    {
        return !m_presentables.empty() && &presentable == *m_presentables.rbegin();
    }

    void Scene::Present(Presentable& presentable, const PresentationContext& context)
    {
        if (auto [_, success] = m_presentables.insert(&presentable); success)
        {
            if (&context == &PresentationContext::Default)
            {
                auto ctx = GraphicalPresentationContext();
                ctx.Bounds      = sf::FloatRect{{0, 0}, GetView().getSize()};
                ctx.IsCentered  = true;

                Parent::Present(presentable, ctx);
            }
            else
                Parent::Present(presentable, context);

            if (const auto inputable = dynamic_cast<Inputable*>(&presentable))
            {
                if (m_lastInput.has_value())
                    inputable->Input(m_lastInput.value());
            }
        }
    }

    bool Scene::Dismiss(Presentable& presentable)
    {
        if (!IsPresenting(presentable))
            return false;

        if (m_presentables.erase(&presentable) > 0)
        {
            Parent::Dismiss(presentable);

            // Apply last mouse movement input so that objects are properly highlighted after dismissing the presentable
            if (m_lastInput.has_value())
                Input(m_lastInput.value());

            return true;
        }

        return false;
    }

    bool Scene::Dismiss()
    {
        return Dismiss(**m_presentables.rbegin());
    }

    void Scene::QueueEvent(const std::function<void()>& evt)
    {
        if (evt)
            m_events.push(evt);
    }

    void Scene::ProcessEvents()
    {
        const auto& director = GetDirector();
        while (!m_events.empty())
        {
            auto event = m_events.front();
            m_events.pop();

            if (event)
                event();

            if (&director.GetPresentingScene() != this)
                return;
        }
    }

    RenderStates Scene::Render(RenderSurface& surface, RenderStates states) const
    {
        states = RenderableContainer::Render(surface, states);
        if (!m_presentables.empty())
        {
            for (const auto presentable : m_presentables)
            {
                if (const auto renderable = dynamic_cast<Renderable*>(presentable))
                    renderable->Render(surface, states);
            }
        }

        return states;
    }

    void Scene::Update(const double delta)
    {
        UpdatableContainer::Update(delta);
        TaskContainer::Update(delta);

        if (!m_presentables.empty())
        {
            for (const auto presentable : m_presentables)
            {
                if (const auto updatable = dynamic_cast<Updatable*>(presentable))
                    updatable->Update(delta);
            }
        }
    }

    bool Scene::Input(const sf::Event& ev)
    {
        if (ev.is<sf::Event::MouseMoved>())
            m_lastInput = ev;

        if (!m_presentables.empty())
        {
            if (const auto inputable = dynamic_cast<Inputable*>(*m_presentables.rbegin()))
                return inputable->Input(ev);

            return false;
        }

        return InputableContainer::Input(ev);
    }
}
