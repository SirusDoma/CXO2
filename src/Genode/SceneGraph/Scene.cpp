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
        m_overlay(nullptr),
        m_deferrables()
    {
        SetName(name);
        SetTag("Scene");
    }

    Scene::~Scene()
    {
        m_overlay = nullptr;
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

    void Scene::SetOverlay(Node *overlay)
    {
        m_overlay = overlay;
    }

    Node *Scene::GetOverlay() const
    {
        return m_overlay;
    }

    void Scene::CloseOverlay()
    {
        SetOverlay(nullptr);
    }

    sf::RenderStates Scene::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        m_view = target.getView();
        states = RenderableContainer::Render(target, states);
        if (m_overlay)
        {
            auto renderable = dynamic_cast<Renderable*>(m_overlay);
            if (renderable)
                states = renderable->Render(target, states);
        }

        //ProcessDeferrables();
        return states;
    }

    void Scene::Update(double delta)
    {
        if (m_overlay)
        {
            auto updatable = dynamic_cast<Updatable*>(m_overlay);
            if (updatable)
                updatable->Update(delta);
        }

        UpdatableContainer::Update(delta);
        TaskContainer::Update(delta);
    }

    bool Scene::Input(sf::Event ev)
    {
        if (m_overlay)
        {
            auto inputable = dynamic_cast<Inputable*>(m_overlay);
            if (inputable)
                return inputable->Input(ev);

            return false;
        }

        return InputableContainer::Input(ev);
    }

    void Scene::ProcessDeferrables() const
    {
        while (!m_deferrables.empty())
        {
            auto callback = m_deferrables.front();
            callback();

            m_deferrables.pop();
        }
    }
}