#include <algorithm>
#include <cstdarg>

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>

namespace Gx
{
    Scene::Scene() :
        Scene::Scene(typeid(this).name())
    {
    }

    Scene::Scene(const std::string &name) :
        Node(),
        RenderableContainer(),
        UpdatableContainer(),
        InputableContainer(),
        m_director(nullptr),
        m_events(),
        m_overlays(),
        m_entities()
    {
        SetName(name);
        SetTag("Scene");
        RegisterLocalResource<ResourceManager>();
    }

    Scene::Scene(ResourceManager& resources) :
        Scene::Scene(typeid(this).name(), resources)
    {
    }

    Scene::Scene(const std::string &name, ResourceManager& resources) :
        Scene::Scene(name)
    {
       m_resources = std::unique_ptr<ResourceManager>(&resources);
    }

    Scene::~Scene()
    {
        m_entities.clear();
        m_sources.clear();
        m_overlays.clear();
    }

    void Scene::Initialize()
    {
    }

    bool Scene::Close(bool quit)
    {
        return true;
    }

    Application &Scene::GetApplication() const
    {
        return m_director->GetApplication();
    }

    SceneDirector& Scene::GetDirector() const
    {
        return *m_director;
    }

    void Scene::SetDirector(SceneDirector& director)
    {
        m_director = &director;
    }

    sf::View Scene::GetView() const
    {
        return m_view;
    }

    void Scene::SetView(const sf::View &view)
    {
        m_view = view;
    }

    ResourceManager &Scene::GetLocalResources() const
    {
        return *m_resources;
    }

    void Scene::PushOverlay(Node *overlay)
    {
        if (overlay)
        {
            AddChild(overlay);
            m_overlays.push_back(overlay);
        }
    }

    void Scene::CloseOverlay()
    {
        if (!m_overlays.empty())
        {
            RemoveChild(m_overlays.back());
            m_overlays.pop_back();
        }

        Input(m_lastInput);
    }

    void Scene::QueueSceneEvent(std::function<void()> evt)
    {
        if (evt)
            m_events.push(evt);
    }

    void Scene::ProcessSceneEvents()
    {
        auto& director = GetDirector();
        while (!m_events.empty())
        {
            auto event = m_events.front();
            m_events.pop();

            if (event)
                event();

            if (director.GetScene() != this)
                return;
        }
    }

    bool Scene::Destroy(Node *resource)
    {
        if (!resource)
            return false;

        auto iterator = std::find_if(m_entities.begin(), m_entities.end(), [resource] (const auto& e) { return resource == e.get(); });
        if (iterator != m_entities.end())
        {
            RemoveChild(resource);
            return m_entities.erase(iterator) == m_entities.end();
        }

        return false;
    }

    sf::RenderStates Scene::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        m_view = target.getView();
        states = RenderableContainer::Render(target, states);

        if (!m_overlays.empty())
        {
            for (auto overlay : m_overlays)
            {
                auto renderable = dynamic_cast<Renderable *>(overlay);
                if (renderable)
                    renderable->Render(target, states);
            }
        }

        return states;
    }

    void Scene::Update(double delta)
    {
        UpdatableContainer::Update(delta);
        TaskContainer::Update(delta);
    }

    bool Scene::Input(sf::Event ev)
    {
        m_lastInput = ev;
        if (!m_overlays.empty())
        {
            auto inputable = dynamic_cast<Inputable*>(m_overlays.back());
            if (inputable)
                return inputable->Input(ev);

            return false;
        }

        return InputableContainer::Input(ev);
    }
}
