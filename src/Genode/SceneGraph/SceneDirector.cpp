#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/SceneGraph/Scene.hpp>

#include <Genode/System/Application.hpp>

namespace Gx
{
    SceneDirector::SceneDirector(SceneDirector &&director) noexcept :
        m_application(director.m_application),
        m_factories(std::move(director.m_factories)),
        m_caches(std::move(director.m_caches)),
        m_currentScene(std::move(director.m_currentScene)),
        m_nextScene(std::move(director.m_nextScene)),
        m_cacheEnabled(),
        m_staged()
    {
    }

    SceneDirector::SceneDirector(Application& app, Scene& scene) :
        m_application(&app),
        m_factories(),
        m_caches(),
        m_currentScene(&scene),
        m_cacheEnabled(),
        m_staged()
    {
    }

    SceneDirector::SceneDirector(Application& app) :
        m_application(&app),
        m_factories(),
        m_caches(),
        m_currentScene(),
        m_cacheEnabled(),
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

            const sf::RenderTarget& target = *m_application;
            m_currentScene->SetView(target.getView());

            m_currentScene->Initialize();
            m_staged = true;
        }
    }

    void SceneDirector::Unstage()
    {
        if (m_currentScene)
            m_currentScene->Close(false);
    }

    bool SceneDirector::IsCacheEnabled() const
    {
        return m_cacheEnabled;
    }

    void SceneDirector::SetCacheEnabled(bool cacheEnabled)
    {
        m_cacheEnabled = cacheEnabled;
    }

    Scene& SceneDirector::GetPresentedScene() const
    {
        return *m_currentScene;
    }

    Application& SceneDirector::GetApplication() const
    {
        if (!m_application)
            return Application::Instance();

        return *m_application;
    }

    RenderStates SceneDirector::Render(RenderSurface& surface, RenderStates states) const
    {
        if (m_currentScene)
            return m_currentScene->Render(surface, states);

        return states;
    }

    void SceneDirector::Update(const double delta)
    {
        Stage();

        if (m_currentScene)
            m_currentScene->Update(delta);
    }

    bool SceneDirector::Input(const sf::Event& ev)
    {
        Stage();

        if (m_currentScene)
            return m_currentScene->Input(ev);

        return false;
    }

    void SceneDirector::ProcessEvents()
    {
        if (m_currentScene)
            m_currentScene->ProcessEvents();
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
