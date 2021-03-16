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

    void Scene::Update(double delta)
    {
        UpdatableContainer::Update(delta);
        TaskContainer::Update(delta);
    }
}