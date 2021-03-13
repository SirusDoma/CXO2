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
        Node::Node(),
        m_director(nullptr)
    {
        SetName(name);
        SetTag("Scene");
    }

    Scene::~Scene()
    {
    }

    void Scene::Initialize()
    {
    }

    bool Scene::Close(bool quit)
    {
        StopAll();
        for (auto child : GetChildren())
            RemoveChild(child);

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

    void Scene::Render(Node* node, sf::RenderTarget& target, sf::RenderStates states) const
    {
        auto renderable = dynamic_cast<Renderable*>(node);
        if (renderable)
        {
            states = renderable->Render(target, states);
            for (auto child : node->GetChildren())
            {
                Render(child, target, states);
            }
        }
    }

    void Scene::Update(Node* node, double delta)
    {
        auto updatable = dynamic_cast<Updatable*>(node);
        if (updatable)
        {
            updatable->Update(delta);
            for (auto child : node->GetChildren())
            {
                Update(child, delta);
            }
        }
    }

    void Scene::Input(Node* node, sf::Event ev)
    {
        auto inputable = dynamic_cast<Inputable*>(node);
        if (inputable)
        {
            inputable->Input(ev);
            for (auto child : node->GetChildren())
            {
                Input(child, ev);
            }
        }
    }

    sf::RenderStates Scene::Render(sf::RenderTarget& target, sf::RenderStates states) const
    {
        for (auto node : GetChildren())
        {
            Render(node, target, states);
        }

        return states;
    }

    void Scene::Update(double delta)
    {
        TaskContainer::Update(delta);
        for (auto node : GetChildren())
        {
            Update(node, delta);
        }
    }

    bool Scene::Input(sf::Event ev)
    {
        if (Inputable::Input(ev))
        {
            for (auto node : GetChildren())
            {
                Input(node, ev);
            }

            return true;
        }

        return false;
    }
}