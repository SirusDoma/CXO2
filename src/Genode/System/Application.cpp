#include <Genode/System/Application.hpp>
#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/IO/Cache.hpp>

namespace Gx
{
    Application* Application::instance;

    Application::Application(Scene* scene, sf::VideoMode mode, bool fullScreen)
        : Application::Application(scene, mode, mode, fullScreen)
    {
    }

    Application::Application(Scene* scene, sf::VideoMode mode, sf::VideoMode virtualMode, bool fullScreen) :
        m_window(mode, Application::TITLE, fullScreen ? sf::Style::Fullscreen : sf::Style::Titlebar | sf::Style::Close),
        m_timer(),
        m_renderFreq(0),
        m_frames(0),
        m_cursor(),
        m_cursorFrame(0)
    {
        m_mode        = mode;
        m_virtualMode = virtualMode;
        m_fullScreen  = fullScreen;

        m_director    = new SceneDirector(scene);
    }

    Application::~Application()
    {
        delete m_director;
    }

    Application* Application::Instance()
    {
        return Application::instance;
    }

    int Application::Start()
    {
        // Set render frequency
        m_window.setVerticalSyncEnabled(true);
        m_window.setFramerateLimit(60);

        // Setup view
        auto view = m_window.getDefaultView();
        view.setSize(static_cast<float>(m_virtualMode.width), static_cast<float>(m_virtualMode.height));
        view.setCenter(m_virtualMode.width / 2.0f, m_virtualMode.height / 2.0f);
        m_window.setView(view);

        // Singleton instancing
        Application::instance = this;

        // Trigger callback
        OnStart();

        // Setup timer
        m_timer = sf::Clock();
        double start = m_timer.getElapsedTime().asMilliseconds(), end, delta, fpsDelta = 0;

        // Main game loop
        while (m_window.isOpen())
        {
            // Poll window event
            sf::Event ev;
            while (m_window.pollEvent(ev))
            {
                // Call window event handlers based on received event
                switch (ev.type)
                {
                    case sf::Event::Closed:      OnClose();             break;
                    case sf::Event::GainedFocus: OnFocusChanged(true);  break;
                    case sf::Event::LostFocus:   OnFocusChanged(false); break;
                    case sf::Event::Resized:     OnResized(ev.size);    break;
                    default:                     OnInputReceived(ev);   break;
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
            m_window.clear();
            {
                // Game routine (update + render)
                m_director->Update(delta);
                m_director->Render(m_window, sf::RenderStates::Default);
            }
            m_window.display();

            // Track the number frames rendered in a second
            fpsDelta += delta;
            if (fpsDelta >= 1000)
            {
                m_renderFreq = m_frames;
                m_frames = 0;

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

    unsigned int Application::GetRenderFrequency() const
    {
        return m_renderFreq;
    }

    void Application::SetCursor(const Cursor& cursor)
    {
        m_cursor = cursor;
        m_window.setMouseCursor(*m_cursor.GetHandle());
    }

    void Application::OnStart()
    {
    }

    void Application::OnClose()
    {
        // Ask game permission first before closing
        if (m_director->Close()) 
            m_window.close();
    }

    void Application::OnInputReceived(sf::Event ev)
    {
        // Pass input into active scene via director
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
}