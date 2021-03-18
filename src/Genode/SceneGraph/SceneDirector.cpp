#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/SceneGraph/Scene.hpp>

namespace Gx
{
    SceneDirector::SceneDirector(Scene* scene, sf::RenderTarget *target) :
        m_scene(),
        m_target(target)
    {
        SetScene(scene);
    }

    SceneDirector::~SceneDirector()
    {
    }

    Scene* SceneDirector::GetScene() const
    {
        return m_scene.get();
    }

    void SceneDirector::SetScene(Scene* scene)
    {
        if (m_scene)
            m_scene->Close();

        m_scene = std::unique_ptr<Scene>(scene);
        if (scene)
        {
            m_scene->SetDirector(this);
            if (m_target)
                m_scene->SetView(m_target->getView());

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