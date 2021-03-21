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
        m_overlays()
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
    }

    bool Scene::Close(bool quit)
    {
        return true;
    }

    SceneDirector* Scene::GetDirector() const
    {
        return m_director;
    }

    void Scene::SetDirector(SceneDirector* director)
    {
        m_director = director;
    }

    sf::View Scene::GetView() const
    {
        return m_view;
    }

    void Scene::SetView(const sf::View &view)
    {
        m_view = view;
    }

    void Scene::PushOverlay(Node *overlay)
    {
        m_overlays.push_back(overlay);
    }

    void Scene::CloseOverlay()
    {
        if (!m_overlays.empty())
            m_overlays.pop_back();
    }

    void Scene::QueueEvent(std::function<void()> evt)
    {
        if (evt)
            m_events.push(evt);
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
        if (!m_overlays.empty())
        {
            for (auto overlay : m_overlays)
            {
                auto updatable = dynamic_cast<Updatable*>(overlay);
                if (updatable)
                    updatable->Update(delta);
            }
        }

        UpdatableContainer::Update(delta);
        TaskContainer::Update(delta);
    }

    bool Scene::Input(sf::Event ev)
    {
        if (!m_overlays.empty())
        {
            auto inputable = dynamic_cast<Inputable*>(m_overlays.back());
            if (inputable)
                return inputable->Input(ev);

            return false;
        }

        return InputableContainer::Input(ev);
    }

    void Scene::ProcessEvents()
    {
        auto director = GetDirector();
        while (!m_events.empty())
        {
            auto event = m_events.front();
            m_events.pop();

            if (event)
                event();

            if (director->GetScene() != this)
                return;
        }
    }
}