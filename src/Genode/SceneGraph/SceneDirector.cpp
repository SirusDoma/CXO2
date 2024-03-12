#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/SceneGraph/Scene.hpp>

namespace Gx
{
    SceneDirector::SceneDirector(SceneDirector &&director) :
        m_application(director.m_application),
        m_target(director.m_target),
        m_currentScene(std::move(director.m_currentScene)),
        m_nextScene(std::move(director.m_nextScene)),
        m_factories(director.m_factories),
        m_caches(std::move(director.m_caches)),
        m_staged()
    {
    }

    SceneDirector::SceneDirector(Application &app, sf::RenderTarget &target, Scene &scene) :
        m_application(&app),
        m_target(&target),
        m_currentScene(&scene),
        m_factories(),
        m_caches(),
        m_staged()
    {
    }

    SceneDirector::SceneDirector(Application &app, sf::RenderTarget &target) :
        m_application(&app),
        m_target(&target),
        m_currentScene(),
        m_factories(),
        m_caches(),
        m_staged()
    {
    }

    SceneDirector::~SceneDirector()
    {
    }

    void SceneDirector::Stage()
    {
        if (m_nextScene && !m_staged)
        {
            m_currentScene = std::move(m_nextScene);
            m_nextScene = nullptr;

            m_currentScene->SetDirector(*this);
            if (m_target)
                m_currentScene->SetView(m_target->getView());

            m_currentScene->Initialize();
            m_staged = true;
        }
    }

    void SceneDirector::Unstage()
    {
        if (m_currentScene)
            m_currentScene->Close();
    }

    bool SceneDirector::IsCacheEnabled() const
    {
        return m_cacheEnabled;
    }

    void SceneDirector::SetCacheEnabled(bool cacheEnabled)
    {
        m_cacheEnabled = cacheEnabled;
    }

    Scene &SceneDirector::GetPresentedScene() const
    {
        return *m_currentScene;
    }

    Application &SceneDirector::GetApplication() const
    {
        return *m_application;
    }

    sf::RenderStates SceneDirector::Render(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_currentScene)
            return m_currentScene->Render(target, states);

        return states;
    }

    void SceneDirector::Update(double delta)
    {
        Stage();

        if (m_currentScene)
            m_currentScene->Update(delta);
    }

    bool SceneDirector::Input(sf::Event ev)
    {
        Stage();

        if (m_currentScene)
            return m_currentScene->Input(ev);

        return false;
    }

    void SceneDirector::ProcessEvents()
    {
        if (m_currentScene)
            m_currentScene->ProcessSceneEvents();
    }

    bool SceneDirector::Close()
    {
        if (m_currentScene)
            return m_currentScene->Close(true);

        return true;
    }

    void SceneDirector::Unload()
    {
        m_currentScene = nullptr;
        m_nextScene = nullptr;
    }
}
