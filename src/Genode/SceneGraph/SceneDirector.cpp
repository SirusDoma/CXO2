#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/SceneGraph/Scene.hpp>

namespace Gx
{
    SceneDirector::SceneDirector(Scene* scene) :
        m_scene(0)
    {
        SetScene(scene);
    }

    SceneDirector::~SceneDirector()
    {
        if (m_scene)
            delete m_scene;
    }

    Scene* SceneDirector::GetScene() const
    {
        return m_scene;
    }

    void SceneDirector::SetScene(Scene* scene)
    {
        if (m_scene)
        {
            m_scene->Close();
            delete m_scene;
        }

        if (scene)
        {
            m_scene = scene;
            m_scene->SetDirector(this);
            m_scene->Initialize();
        }
    }

    sf::RenderStates SceneDirector::Render(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_scene)
            return m_scene->Render(target, states);

        return states;
    }

    void SceneDirector::Update(double delta)
    {
        if (m_scene)
            m_scene->Update(delta);
    }

    bool SceneDirector::Input(sf::Event ev)
    {
        if (m_scene)
            m_scene->Input(ev);

        return false;
    }

    bool SceneDirector::Close()
    {
        if (m_scene)
            return m_scene->Close(true);

        return true;
    }
}