#include <Genode/System/Application.hpp>
#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/System/Provider.hpp>

namespace Gx
{
    Application::Application(const std::string &title, const sf::VideoMode &mode, const bool fullScreen)
        : Application(title, mode, mode, fullScreen)
    {
    }

    Application::Application(const std::string &title, const sf::VideoMode &mode, const sf::VideoMode &virtualMode, const bool fullScreen) :
        m_window(std::make_unique<sf::RenderWindow>(mode, title, sf::Style::Titlebar | sf::Style::Close, fullScreen ? sf::State::Fullscreen : sf::State::Windowed)),
        m_targetAdapter(*this),
        m_director(SceneDirector(*this)),
        m_event(),
        m_state(fullScreen ? sf::State::Fullscreen : sf::State::Windowed),
        m_resources(),
        m_timer(),
        m_cursor(),
        m_title(title),
        m_frameID(0),
        m_frames(0),
        m_renderFreq(0)
    {
        m_mode           = mode;
        m_virtualMode    = virtualMode;
        m_fullScreen     = fullScreen;
        m_closeRequested = false;
    }

    Application &Application::Instance()
    {
        if (!m_instance)
            throw Exception("Application is not instantiated yet.");

        return *m_instance;
    }

    int Application::Start()
    {
        if (m_instance && m_instance != this)
            throw Exception("Only single application instance allowed");

        // Initialize application instance and director
        m_instance = this;

        // Prepare window
        SetupWindow();

        // Bootstrap the game
        Boot();

        // Setup timer
        m_timer = sf::Clock();
        double start = m_timer.getElapsedTime().asMilliseconds(), fpsDelta = 0;

        // Main game loop
        while (m_window->isOpen())
        {
            // Poll window event
            auto event = sf::Event();
            while (m_window->pollEvent(event))
            {
                // Call window event handlers based on received event
                switch (event.type)
                {
                    case sf::Event::Closed:      OnClose();              break;
                    case sf::Event::GainedFocus: OnFocusChanged(true);   break;
                    case sf::Event::LostFocus:   OnFocusChanged(false);  break;
                    case sf::Event::Resized:     OnResized(event.size);  break;
                    default:                     OnInputReceived(event); break;
                }
            }

            // Check if window is closed after polling the events
            if (m_closeRequested || !m_window->isOpen())
            {
                m_window->close();
                break;
            }

            // Calculate delta
            const double end = m_timer.getElapsedTime().asMilliseconds();
            const double delta = end - start;

            // Update installed modules
            for (auto& [_, context] : m_providers)
            {
                if (const auto updatable = dynamic_cast<Updatable*>(context.get()))
                    updatable->Update(delta);
            }

            // Perform update before rendering objects
            Update(delta);

            // Render the window
            m_window->clear(sf::Color::White);
            {
                // Render objects
                Render(*this, Gx::RenderStates(sf::RenderStates::Default, m_frameID++, delta));
            }
            m_window->display();

            // Execute post-processing events
            m_director.ProcessEvents();

            // Track the number of frames rendered in a second
            fpsDelta += delta;
            if (fpsDelta >= 1000)
            {
                m_renderFreq = m_frames;
                m_frames = 0;
                m_window->setTitle(m_title + " [FPS: " + std::to_string(m_renderFreq) + "]");

                fpsDelta = 0.0f;
            }

            // Update starting point of delta time
            start = end;

            // Update fps counter
            m_frames++;
        }

        // Clean up
        Shutdown();

        // Application exit code
        return 0;
    }

    int Application::Start(Scene &scene)
    {
        m_director.Present(scene);
        return Start();
    }

    sf::RenderWindow &Application::GetRenderWindow() const
    {
        return *m_window;
    }

    SceneDirector &Application::GetSceneDirector() const
    {
        return m_director;
    }

    void Application::Boot()
    {
    }

    void Application::Shutdown()
    {
    }

    void Application::Update(const double delta)
    {
        m_director.Update(delta);
    }

    RenderStates Application::Render(RenderSurface &surface, RenderStates states) const
    {
        return m_director.Render(surface, states);
    }

    void Application::Close()
    {
        OnClose();
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
        if (m_state == state)
            return;
        
        m_window->close();
        m_window = std::make_unique<sf::RenderWindow>(m_mode, m_title, sf::Style::Titlebar | sf::Style::Close, state);
        m_state  = state;

        SetupWindow();
    }

    void Application::SetCursor(Cursor& cursor)
    {
        m_cursor = &cursor;
        m_window->setMouseCursor(m_cursor->GetHandle());
    }

    void Application::OnFocusChanged(bool focus)
    {
    }

    void Application::OnResized(sf::Event::SizeEvent ev)
    {
    }

    void Application::OnInputReceived(sf::Event ev)
    {
        // Re-map mouse coordinate when using virtual mode
        if (m_mode != m_virtualMode)
        {
            switch (ev.type)
            {
                case sf::Event::MouseMoved:
                {
                    auto position = m_window->mapPixelToCoords(sf::Vector2i(ev.mouseMove.x, ev.mouseMove.y));
                    ev.mouseMove = sf::Event::MouseMoveEvent{
                        static_cast<int>(position.x),
                        static_cast<int>(position.y)
                    };

                    break;
                }
                case sf::Event::MouseButtonPressed:
                case sf::Event::MouseButtonReleased:
                {
                    auto position = m_window->mapPixelToCoords(sf::Vector2i(ev.mouseButton.x, ev.mouseButton.y));
                    ev.mouseButton = sf::Event::MouseButtonEvent{
                        ev.mouseButton.button,
                        static_cast<int>(position.x),
                        static_cast<int>(position.y)
                    };

                    break;
                }
                case sf::Event::MouseWheelScrolled:
                {
                    auto position = m_window->mapPixelToCoords(sf::Vector2i(ev.mouseWheelScroll.x, ev.mouseWheelScroll.y));
                    ev.mouseWheelScroll = sf::Event::MouseWheelScrollEvent{
                        ev.mouseWheelScroll.wheel,
                        ev.mouseWheelScroll.delta,
                        static_cast<int>(position.x),
                        static_cast<int>(position.y)
                    };

                    break;
                }
                default:
                    break;
            }
        }

        // Pass input into active scene via director
        m_director.Input(ev);

        //// Move cursor
        //if (ev.type == sf::Event::MouseMoved)
        //    m_cursor.setPosition(static_cast<float>(ev.mouseMove.x), static_cast<float>(ev.mouseMove.y));

        //// Mouse click
        //if (ev.type == sf::Event::MouseButtonPressed && m_cursorFrame)
        //    m_cursor.setTextureRect(sf::IntRect(m_cursorFrame->width, 0, m_cursorFrame->width, m_cursorFrame->height));

        //// Mouse release
        //if (ev.type == sf::Event::MouseButtonReleased && m_cursorFrame)
        //    m_cursor.setTextureRect(sf::IntRect(0, 0, m_cursorFrame->width, m_cursorFrame->height));
    }

    void Application::OnClose()
    {
        // Ask game permission first before closing
        if (m_director.Close())
            m_closeRequested = true;
    }

    void Application::SetupWindow() const
    {
        // Set render frequency
        m_window->setVerticalSyncEnabled(true);

        // Setup view
        auto view = m_window->getDefaultView();
        view.setSize(sf::Vector2f(static_cast<float>(m_virtualMode.size.x), static_cast<float>(m_virtualMode.size.y)));
        view.setCenter(sf::Vector2f(m_virtualMode.size.x / 2.0f, m_virtualMode.size.y / 2.0f));
        m_window->setView(view);

        m_targetAdapter = RenderTargetAdapter(*this);
    }

    Application::operator sf::RenderTarget&() const
    {
        return *m_window;
    }

    Application::operator sf::RenderWindow&() const
    {
        return *m_window;
    }

    Application::operator RenderSurface&() const
    {
        return m_targetAdapter;
    }
}
