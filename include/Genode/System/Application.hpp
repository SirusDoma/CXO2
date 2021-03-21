#ifndef GENODE_APPLICATION_HPP
#define GENODE_APPLICATION_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <Genode/System/Module.hpp>
#include <Genode/Graphics/Cursor.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>


namespace Gx
{
    class Scene;
    class SceneDirector;
    class Application
    {
    public:
        const sf::String TITLE = "O2-JAM";
        static Application* Instance();

        Application(Scene* scene, sf::VideoMode mode, bool fullScreen = false);
        Application(Scene* scene, sf::VideoMode mode, sf::VideoMode virtualMode, bool fullScreen = false);
        ~Application();

        int Start();
        void Close();

        unsigned int GetRenderFrequency() const;
        void SetCursor(const Cursor& cursor);

        template<typename T>
        bool Install();

        template<typename T>
        bool Install(T* instance);

        template<typename T>
        bool Uninstall();

        template<typename T>
        T* GetModule() const;

    protected:
        virtual void OnStart();
        virtual void OnClose();

        virtual void OnFocusChanged(bool focus);
        virtual void OnResized(sf::Event::SizeEvent ev);

        virtual void OnInputReceived(sf::Event ev);

    private:
        static Application* instance;
        SceneDirector* m_director;

        sf::RenderWindow m_window;
        sf::VideoMode m_mode;
        sf::VideoMode m_virtualMode;

        sf::Clock m_timer;
        Cursor m_cursor;
        const sf::IntRect* m_cursorFrame;

        std::vector<Module*> m_modules;

        unsigned int m_frames;
        unsigned int m_renderFreq;
        bool m_fullScreen;
    };
}

#include <Genode/System/Application.inl>
#endif