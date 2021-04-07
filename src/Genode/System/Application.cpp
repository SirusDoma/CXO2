#include <Genode/System/Application.hpp>
#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>

namespace Gx
{
    Application::Application(sf::VideoMode mode, bool fullScreen)
        : Application::Application(mode, mode, fullScreen)
    {
    }

    Application::Application(sf::VideoMode mode, sf::VideoMode virtualMode, bool fullScreen) :
        m_window(mode, Application::TITLE, fullScreen ? sf::Style::Fullscreen : sf::Style::Titlebar | sf::Style::Close),
        m_timer(),
        m_renderFreq(0),
        m_frames(0),
        m_cursor(),
        m_resources(),
        m_mixer()
    {
        m_mode        = mode;
        m_virtualMode = virtualMode;
        m_fullScreen  = fullScreen;
    }

    Application::~Application()
    {
        delete m_director;
    }

    int Application::Start(Scene *scene)
    {
        // Construct resources
        m_director = new SceneDirector(*this, scene, m_window);

        // Set render frequency
        m_window.setVerticalSyncEnabled(true);
        m_window.setFramerateLimit(60);

        // Setup view
        auto view = m_window.getDefaultView();
        view.setSize(static_cast<float>(m_virtualMode.width), static_cast<float>(m_virtualMode.height));
        view.setCenter(m_virtualMode.width / 2.0f, m_virtualMode.height / 2.0f);
        m_window.setView(view);

        // Trigger callback
        OnStart();

        // Fallback mixer and resources
        if (!m_director->m_resources)
            ShareResources(m_resources);

        if (!m_director->m_mixer)
            UseMixer(m_mixer);

        // Initialize scene
        m_director->Initialize();

        // Setup timer
        m_timer = sf::Clock();
        double start = m_timer.getElapsedTime().asMilliseconds(), end, delta, fpsDelta = 0;

        // Main game loop
        while (m_window.isOpen())
        {
            // Poll window event
            m_event = sf::Event();
            while (m_window.pollEvent(m_event))
            {
                // Call window event handlers based on received event
                switch (m_event.type)
                {
                    case sf::Event::Closed:      OnClose();             break;
                    case sf::Event::GainedFocus: OnFocusChanged(true);  break;
                    case sf::Event::LostFocus:   OnFocusChanged(false); break;
                    case sf::Event::Resized:     OnResized(m_event.size);    break;
                    default:                     OnInputReceived(m_event);   break;
                }
            }

            // Calculate delta
            end   = m_timer.getElapsedTime().asMilliseconds();
            delta = end - start;

            // Update installed modules
            for (auto mod : m_modules)
            {
                if (mod && mod->CheckFrequency(delta))
                    mod->Update(delta);
            }

            // Render the window
            m_window.clear(sf::Color::White);
            {
                // Game routine (update + render)
                m_director->Update(delta);
                m_director->Render(m_window, sf::RenderStates::Default);
            }
            m_window.display();

            m_director->ProcessEvents();

            // Track the number of frames rendered in a second
            fpsDelta += delta;
            if (fpsDelta >= 1000)
            {
                m_renderFreq = m_frames;
                m_frames = 0;
                m_window.setTitle(TITLE + " [FPS: " + std::to_string(m_renderFreq) + "]");

                fpsDelta = 0.0f;
            }

            // Update starting point of delta time
            start = end;

            // Update fps counter
            m_frames++;
        }

        // Application exit code
        return 0;
    }

    sf::RenderWindow &Application::GetRenderWindow() const
    {
        return m_window;
    }

    sf::Event Application::GetLastEvent() const
    {
        return m_event;
    }

    void Application::ShareResources(ResourceManager &resources)
    {
        if (m_director)
            m_director->SetSharedResources(resources);
    }


    void Application::UseMixer(Mixer &mixer)
    {
        if (m_director)
            m_director->SetMixer(mixer);
    }

    void Application::OnStart()
    {
        ShareResources(m_resources);
    }

    void Application::Close()
    {
        OnClose();
    }

    unsigned int Application::GetRenderFrequency() const
    {
        return m_renderFreq;
    }

    void Application::SetCursor(const Cursor& cursor)
    {
        m_cursor = cursor;
        m_window.setMouseCursor(*m_cursor.GetHandle());
    }

    void Application::OnClose()
    {
        // Ask game permission first before closing
        if (m_director->Close()) 
            m_window.close();
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
                    auto position = m_window.mapPixelToCoords(sf::Vector2i(ev.mouseMove.x, ev.mouseMove.y));
                    ev.mouseMove = sf::Event::MouseMoveEvent{
                        static_cast<int>(position.x),
                        static_cast<int>(position.y)
                    };

                    break;
                }
                case sf::Event::MouseButtonPressed:
                case sf::Event::MouseButtonReleased:
                {
                    auto position = m_window.mapPixelToCoords(sf::Vector2i(ev.mouseButton.x, ev.mouseButton.y));
                    ev.mouseButton = sf::Event::MouseButtonEvent{
                        ev.mouseButton.button,
                        static_cast<int>(position.x),
                        static_cast<int>(position.y)
                    };

                    break;
                }
                case sf::Event::MouseWheelScrolled:
                {
                    auto position = m_window.mapPixelToCoords(sf::Vector2i(ev.mouseWheelScroll.x, ev.mouseWheelScroll.y));
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
        m_event = ev;
        m_director->Input(ev);

        // Move cursor
        //if (ev.type == sf::Event::MouseMoved)
        //    m_cursor.setPosition(static_cast<float>(ev.mouseMove.x), static_cast<float>(ev.mouseMove.y));

        //// Mouse click
        //if (ev.type == sf::Event::MouseButtonPressed && m_cursorFrame)
        //    m_cursor.setTextureRect(sf::IntRect(m_cursorFrame->width, 0, m_cursorFrame->width, m_cursorFrame->height));

        //// Mouse release
        //if (ev.type == sf::Event::MouseButtonReleased && m_cursorFrame)
        //    m_cursor.setTextureRect(sf::IntRect(0, 0, m_cursorFrame->width, m_cursorFrame->height));
    }

    void Application::OnFocusChanged(bool focus)
    {
    }

    void Application::OnResized(sf::Event::SizeEvent ev)
    {
    }

    Application::operator sf::RenderTarget&() const
    {
        return m_window;
    }
}
