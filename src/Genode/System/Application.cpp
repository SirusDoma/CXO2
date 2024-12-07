#include <Genode/System/Application.hpp>
#include <Genode/System/Context.hpp>
#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/Graphics/Sprite.hpp>
#include <Genode/UI/Cursor.hpp>

#include <mutex>

namespace Gx
{
    Application::Application(const std::string& title, const sf::VideoMode& mode, const bool fullScreen, const sf::ContextSettings& settings)
        : Application(title, mode, sf::View({mode.size.x / 2.f, mode.size.y / 2.f}, {static_cast<float>(mode.size.x), static_cast<float>(mode.size.y)}), fullScreen)
    {
    }

    Application::Application(const std::string& title, const sf::VideoMode& mode, const sf::View& view, const bool fullScreen, const sf::ContextSettings& settings) :
        m_director(SceneDirector(*this)),
        m_context(),
        m_state(fullScreen ? sf::State::Fullscreen : sf::State::Windowed),
        m_mode(mode),
        m_view(view),
        m_settings(settings),
        m_cursor(),
        m_title(title),
        m_frameID(0),
        m_renderFreq(0),
        m_fullScreen(fullScreen),
        m_closeRequested(false)
    {
        CreateMainWindow();
        ResourceLoaderFactory::BindContext(m_context);
    }

    Application& Application::Instance()
    {
        if (!m_instance)
            throw Exception("Application is not instantiated yet");

        return *m_instance;
    }

    int Application::Start()
    {
        {
            static std::recursive_mutex mutex;
            const std::lock_guard lock(mutex);
            if (m_instance && m_instance != this)
                throw Exception("Only single application instance allowed");

            m_instance = this;
        }

        // Bootstrap the game
        Boot();

        // Update cursor handle and scale
        UpdateCursor(sf::Event::Closed());

        // Setup timer
        const auto timer   = sf::Clock();
        double last        = timer.getElapsedTime().asMilliseconds();
        std::size_t frames = 0;
        double fpsDelta    = 0;

        // Main game loop
        bool initial = true;
        while (m_window->isOpen())
        {
            // Poll window event
            while (const auto event = m_window->pollEvent())
            {
                // Call window event handlers based on received event
                if (event.has_value() && !event->is<sf::Event::MouseMovedRaw>())
                {
                    if (event->is<sf::Event::Closed>())
                    {
                        // Ask game permission first before closing
                        Close();
                    }
                    else if (event->is<sf::Event::FocusGained>())
                    {
                        OnFocusChanged(true);
                        m_director.Focus(true);
                    }
                    else if (event->is<sf::Event::FocusLost>())
                    {
                        OnFocusChanged(false);
                        m_director.Focus(false);
                    }
                    else if (const auto e = event->getIf<sf::Event::Resized>())
                    {
                        OnResized(e->size);
                        m_director.Resize(e->size);
                    }
                    else
                    {
                        if (event->is<sf::Event::MouseButtonPressed>() || event->is<sf::Event::MouseButtonReleased>())
                            UpdateCursor(event.value());

                        auto ev = event.value();
                        OnInputReceived(ev);

                        break;
                    }
                }
            }

            // Check if window is closed after polling the events
            if (m_closeRequested || !m_window->isOpen())
            {
                m_window->close();
                break;
            }

            // Calculate delta
            const double now   = timer.getElapsedTime().asMilliseconds();
            const double delta = initial ? 0 : now - last;
            last = now;

            // Update fps counter
            frames++;

            // Track the number of frames rendered in a second
            fpsDelta += delta;
            if (fpsDelta >= 1000)
            {
                m_renderFreq = frames;
                frames       = 0;

                fpsDelta -= 1000.f;
            }

            // Perform update before rendering objects
            Update(delta);

            // Render the window
            m_window->clear(m_clearColor);
            {
                // Render objects
                Render(*this, RenderStates(sf::RenderStates::Default, m_frameID++, delta));
            }
            m_window->display();

            // Execute post-processing events
            m_director.ProcessEvents();

            // Mark initial frame has been processed
            initial = false;
        }

        // Clean up with application exit code
        return Shutdown();
    }

    sf::RenderWindow& Application::GetMainWindow() const
    {
        return *m_window;
    }

    const sf::ContextSettings& Application::GetSettings() const
    {
        return m_settings;
    }

    SceneDirector& Application::GetSceneDirector() const
    {
        return m_director;
    }

    void Application::Boot()
    {
    }

    int Application::Shutdown()
    {
        return 0;
    }

    void Application::Update(const double delta)
    {
        m_director.Update(delta);
    }

    RenderStates Application::Render(RenderSurface& surface, const RenderStates states) const
    {
        surface.Render(m_director, states);
        return states;
    }

    void Application::Close()
    {
        OnClose();

        // Ask game permission first before closing
        m_closeRequested = m_director.Close();
    }

    const std::string& Application::GetTitle() const
    {
        return m_title;
    }

    Context& Application::GetContext() const
    {
        return m_context;
    }

    unsigned int Application::GetRenderFrequency() const
    {
        return m_renderFreq;
    }

    sf::State Application::GetWindowState() const
    {
        return m_state;
    }

    void Application::SetWindowState(const sf::State state)
    {
        if (m_window && m_state == state)
            return;

        m_state = state;
        CreateMainWindow();
    }

    void Application::OnWindowCreated(sf::RenderWindow& window)
    {
    }

    void Application::OnFocusChanged(bool focus)
    {
    }

    void Application::OnResized(const sf::Vector2u& size)
    {
    }

    void Application::OnInputReceived(sf::Event& ev)
    {
        // Re-map mouse coordinate
        if (const auto mv = ev.getIf<sf::Event::MouseMoved>())
        {
            const auto position = m_window->mapPixelToCoords(mv->position);
            ev = sf::Event::MouseMoved
            {
                {
                    static_cast<int>(position.x),
                    static_cast<int>(position.y)
                }
            };
        }
        else if (const auto mp = ev.getIf<sf::Event::MouseButtonPressed>())
        {
            const auto position = m_window->mapPixelToCoords(mp->position);
            ev = sf::Event::MouseButtonPressed
            {
                mp->button,
                {
                    static_cast<int>(position.x),
                    static_cast<int>(position.y)
                }
            };
        }
        else if (const auto mr = ev.getIf<sf::Event::MouseButtonReleased>())
        {
            const auto position = m_window->mapPixelToCoords(mr->position);
            ev = sf::Event::MouseButtonReleased
            {
                mr->button,
                {
                    static_cast<int>(position.x),
                    static_cast<int>(position.y)
                }
            };
        }
        else if (const auto mw = ev.getIf<sf::Event::MouseWheelScrolled>())
        {
            const auto position = m_window->mapPixelToCoords(mw->position);
            ev = sf::Event::MouseWheelScrolled
            {
                mw->wheel,
                mw->delta,
                {
                    static_cast<int>(position.x),
                    static_cast<int>(position.y)
                }
            };
        }

        // Pass input into active scene via director
        m_director.Input(ev);
    }

    void Application::OnClose()
    {
    }

    void Application::CreateMainWindow()
    {
        // Close existing window
        if (m_window)
            m_window->close();

        // Determine video mode to use
        auto mode = m_mode;
        if (m_state == sf::State::Fullscreen)
        {
            if (const auto fsModes = sf::VideoMode::getFullscreenModes(); !fsModes.empty())
                mode = fsModes.front();
            else
                mode = GetDesktopVideoMode();
        }

        // Create/Re-create the window and apply window state.
        // No option to turn into exclusive fullscreen for now.
        m_window = std::make_unique<sf::RenderWindow>(
            mode,
            m_title,
            m_state == sf::State::Fullscreen ? sf::Style::None : sf::Style::Titlebar | sf::Style::Close,
            sf::State::Windowed,
            m_settings
        );

        // This fix windowed to fullscreen issue in X11
        if (m_state == sf::State::Fullscreen)
            m_window->setPosition(sf::Vector2i(0, 0));

        m_window->setVerticalSyncEnabled(true);
        m_window->setView(m_view);

        UpdateCursor(sf::Event::Closed());
        OnWindowCreated(*m_window);
    }

    void Application::UpdateCursor(const sf::Event& ev) const
    {
        if (!m_cursor)
            return;

        auto type = Cursor::Type::Arrow;
        if (const auto mp = ev.getIf<sf::Event::MouseButtonPressed>(); mp && mp->button == sf::Mouse::Button::Left)
            type = Cursor::Type::Click;

        float ratio = static_cast<float>(m_window->getSize().x) / m_window->getView().getSize().x;
        ratio = std::max(static_cast<float>(m_window->getSize().y) / m_window->getView().getSize().y, ratio);

        if (m_cursor->Scale(ratio) || m_cursor->GetLastRetrievedHandleType() != type)
            m_window->setMouseCursor(m_cursor->GetHandle(type));
    }

    const sf::Color& Application::GetClearColor() const
    {
        return m_clearColor;
    }

    void Application::SetClearColor(const sf::Color& clearColor)
    {
        m_clearColor = clearColor;
    }

    Cursor* Application::GetCursor() const
    {
        return m_cursor;
    }

    void Application::SetCursor(Cursor& cursor)
    {
        m_cursor = &cursor;
        m_window->setMouseCursor(m_cursor->GetHandle());
    }

    void Application::InvalidateCursor() const
    {
        m_window->setMouseCursor(m_cursor->GetHandle());
    }

    sf::VideoMode Application::GetCurrentVideoMode() const
    {
        return m_mode;
    }

    const sf::View& Application::GetDefaultView() const
    {
        return m_view;
    }

    const sf::View& Application::GetView() const
    {
        return m_window->getView();
    }

    void Application::SetView(const sf::View& view)
    {
        m_window->setView(view);
    }

    void Application::Clear(const sf::Color clearColor)
    {
        m_window->clear(clearColor);
    }

    void Application::Clear(const sf::Color clearColor, const sf::StencilValue stencilValue)
    {
        m_window->clear(clearColor, stencilValue);
    }

    void Application::Render(const Renderable& renderable, const RenderStates& states)
    {
        renderable.Render(*this, states);
    }

    void Application::Render(const sf::Vertex* vertices, const std::size_t vertexCount, const sf::PrimitiveType type, const RenderStates& states)
    {
        m_window->draw(vertices, vertexCount, type, states);
    }

    void Application::Render(const sf::VertexBuffer& vertexBuffer, const RenderStates& states)
    {
        m_window->draw(vertexBuffer, states);
    }

    void Application::Render(const sf::VertexBuffer& vertexBuffer, const std::size_t firstVertex, const std::size_t vertexCount, const RenderStates& states)
    {
        m_window->draw(vertexBuffer, firstVertex, vertexCount, states);
    }

    Application::operator sf::RenderTarget&() const
    {
        return *m_window;
    }

    Application::operator sf::RenderWindow&() const
    {
        return *m_window;
    }

    sf::VideoMode Application::GetDesktopVideoMode()
    {
        return sf::VideoMode::getDesktopMode();
    }

}
