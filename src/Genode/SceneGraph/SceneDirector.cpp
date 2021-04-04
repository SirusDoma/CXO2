#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/SceneGraph/Scene.hpp>

namespace Gx
{
    SceneDirector::SceneDirector(Application &app, Scene *scene, const sf::RenderTarget &target) :
        m_application(&app),
        m_scene(scene),
        m_target(&target),
        m_resources(),
        m_mixer()
    {
    }

    SceneDirector::~SceneDirector()
    {
    }

    void SceneDirector::Initialize()
    {
        if (m_scene)
        {
            m_scene->SetDirector(*this);
            if (m_target)
                m_scene->SetView(m_target->getView());

            m_scene->Initialize();
        }
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
        Initialize();
    }

    Application &SceneDirector::GetApplication() const
    {
        return *m_application;
    }

    ResourceManager &SceneDirector::GetSharedResources() const
    {
        return *m_resources;
    }

    Mixer &SceneDirector::GetMixer() const
    {
        return *m_mixer;
    }

    void SceneDirector::SetMixer(Mixer &mixer)
    {
        m_mixer = &mixer;
    }

    void SceneDirector::SetSharedResources(ResourceManager &resources)
    {
        m_resources = &resources;
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
            return m_scene->Input(ev);

        return false;
    }

    void SceneDirector::ProcessEvents()
    {
        if (m_scene)
            m_scene->ProcessSceneEvents();
    }

    bool SceneDirector::Close()
    {
        if (m_scene)
            return m_scene->Close(true);

        return true;
    }
}
