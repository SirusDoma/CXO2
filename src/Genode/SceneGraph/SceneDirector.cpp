#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/SceneGraph/Scene.hpp>

#include <Genode/System/Application.hpp>

namespace Gx
{

    SceneDirector::SceneDirector(Application& app) :
        m_surface(app)
    {
    }

    SceneDirector::SceneDirector(RenderSurface& surface) :
        m_surface(surface)
    {
    }

    void SceneDirector::Stage()
    {
        if (m_nextScene && !m_staged)
        {
            m_currentScene = std::move(m_nextScene);
            m_nextScene = nullptr;

            m_currentScene->SetDirector(*this);
            m_currentScene->SetContext(GetContext().Capture());
            m_currentScene->Initialize();

            if (m_initializer)
                m_initializer();

            m_initializer = nullptr;
            m_staged = true;
        }
    }

    void SceneDirector::Unstage() const
    {
        if (m_currentScene)
        {
            m_currentScene->Finalize();
            m_staged = false;
        }
    }

    Context& SceneDirector::GetContext() const
    {
        if (const auto app = dynamic_cast<Application*>(&m_surface))
            return app->GetContext();

        return m_context;
    }

    Scene& SceneDirector::GetPresentingScene() const
    {
        return *m_currentScene;
    }

    const sf::View& SceneDirector::GetView() const
    {
        return m_surface.GetView();
    }

    const sf::View& SceneDirector::GetDefaultView() const
    {
        return m_surface.GetDefaultView();
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void SceneDirector::SetView(const sf::View& view)
    {
        m_surface.SetView(view);
    }

    Application& SceneDirector::GetApplication() const
    {
        if (const auto app = dynamic_cast<Application*>(&m_surface))
            return *app;

        return Application::Instance();
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

    bool SceneDirector::Dismiss()
    {
        if (m_stack.size() <= 1)
            return false;

        m_stack.pop();
        const auto& presentation = m_stack.top();

        ResourceContext* context = nullptr;
        if (presentation.Context)
        {
            context = presentation.Context.get();
            context->Unbind();
        }

        auto scene    = presentation.Deserializer(context ? *context : ResourceContext::Default);
        m_initializer = presentation.Initializer;
        m_nextScene   = std::move(scene);

        Unstage();
        return true;
    }

    bool SceneDirector::Dismiss(const ResourceContext& context)
    {
        if (m_stack.size() <= 1)
            return false;

        m_stack.pop();
        const auto& presentation = m_stack.top();

        auto scene    = presentation.Deserializer(context);
        m_initializer = presentation.Initializer;
        m_nextScene   = std::move(scene);

        Unstage();
        return true;
    }

    void SceneDirector::ProcessEvents() const
    {
        if (m_currentScene)
            m_currentScene->ProcessEvents();
    }

    void SceneDirector::Focus(const bool focused) const
    {
        if (m_currentScene)
            m_currentScene->OnAppFocusChanged(focused);
    }

    void SceneDirector::Resize(const sf::Vector2u& size) const
    {
        if (m_currentScene)
            m_currentScene->OnAppResized(size);
    }

    bool SceneDirector::Close() const
    {
        if (m_currentScene)
        {
            if (m_currentScene->OnAppClose())
            {
                m_currentScene->Finalize();
                return true;
            }

            return false;
        }

        return true;
    }

    void SceneDirector::Reset()
    {
        m_currentScene = nullptr;
        m_nextScene    = nullptr;
        m_staged       = true;
        m_stack      = {};
    }
}
