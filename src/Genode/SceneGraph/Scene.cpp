#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/System/Application.hpp>

namespace Gx
{
    Scene::Scene() :
        Scene::Scene(typeid(this).name())
    {
    }

    Scene::Scene(const std::string& name) :
        Node(),
        RenderableContainer(),
        UpdatableContainer(),
        InputableContainer(),
        m_director(nullptr),
        m_overlays(),
        m_events()
    {
        SetName(name);
        SetTag("Scene");
    }

    Scene::~Scene()
    {
        m_overlays.clear();
    }

    void Scene::Initialize()
    {
        Node::Initialize();
    }

    bool Scene::Close(bool quit)
    {
        return true;
    }

    Application& Scene::GetApplication() const
    {
        if (!m_director)
            return Application::Instance();

        return m_director->GetApplication();
    }

    Context& Scene::GetContext() const
    {
        return GetApplication().GetContext();
    }

    SceneDirector& Scene::GetDirector() const
    {
        if (!m_director)
            throw Exception("SceneDirector is not ready yet");

        return *m_director;
    }

    void Scene::SetDirector(SceneDirector& director)
    {
        m_director =& director;
    }

    sf::View Scene::GetView() const
    {
        return GetApplication().GetView();
    }

    sf::View Scene::GetInitialView() const
    {
        return GetApplication().GetInitialView();
    }

    sf::View Scene::GetDefaultView() const
    {
        const sf::RenderTarget& target = GetApplication();
        return target.getDefaultView();
    }

    void Scene::SetView(const sf::View& view)
    {
        GetApplication().SetView(view);
    }

    Node* Scene::GetCurrentOverlay() const
    {
        if (m_overlays.size() > 0)
            return m_overlays.back();

        return nullptr;
    }

    void Scene::PushOverlay(Node& overlay)
    {
        AddChild(overlay);
        m_overlays.push_back(&overlay);
    }

    void Scene::CloseOverlay()
    {
        if (!m_overlays.empty())
        {
            RemoveChild(*m_overlays.back());
            m_overlays.pop_back();
        }

        Input(m_lastInput.value());
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

            if (&director.GetPresentedScene() != this)
                return;
        }
    }

    RenderStates Scene::Render(RenderSurface& surface, RenderStates states) const
    {
        states = RenderableContainer::Render(surface, states);
        if (!m_overlays.empty())
        {
            for (const auto overlay : m_overlays)
            {
                if (const auto renderable = dynamic_cast<Renderable*>(overlay))
                    renderable->Render(surface, states);
            }
        }

        return states;
    }

    void Scene::Update(const double delta)
    {
        UpdatableContainer::Update(delta);
        TaskContainer::Update(delta);
    }

    bool Scene::Input(const sf::Event& ev)
    {
        m_lastInput = ev;
        if (!m_overlays.empty())
        {
            if (const auto inputable = dynamic_cast<Inputable*>(m_overlays.back()))
                return inputable->Input(ev);

            return false;
        }

        return InputableContainer::Input(ev);
    }
}
