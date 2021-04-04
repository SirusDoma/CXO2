#ifndef GENODE_SYSTEM_APPLICATION_HPP
#define GENODE_SYSTEM_APPLICATION_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <Genode/System/Module.hpp>
#include <Genode/Graphics/Cursor.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Audio/Mixer.hpp>

namespace Gx
{
    class Scene;

    class SceneDirector;

    class Application
    {
    public:
        const std::string TITLE = "O2-JAM";

        Application(sf::VideoMode mode, bool fullScreen = false);
        Application(sf::VideoMode mode, sf::VideoMode virtualMode, bool fullScreen = false);
        ~Application();

        int Start(Scene *scene);
        void Close();

        unsigned int GetRenderFrequency() const;
        sf::Event GetLastEvent() const;
        void SetCursor(const Cursor &cursor);

        template<typename T>
        bool Install();

        template<typename T>
        bool Install(T *instance);

        template<typename T>
        bool Uninstall();

        template<typename T>
        T *GetModule() const;

        operator sf::RenderTarget&() const;

    protected:
        sf::RenderWindow &GetRenderWindow() const;

        virtual void OnStart();
        virtual void OnClose();
        virtual void OnFocusChanged(bool focus);
        virtual void OnResized(sf::Event::SizeEvent ev);
        virtual void OnInputReceived(sf::Event ev);

        void ShareResources(ResourceManager &resources);
        void UseMixer(Mixer &mixer);

    private:
        mutable sf::RenderWindow m_window;
        sf::Event m_event;

        SceneDirector  *m_director;
        ResourceManager m_resources;
        Mixer           m_mixer;

        sf::VideoMode m_mode;
        sf::VideoMode m_virtualMode;

        sf::Clock m_timer;
        Cursor m_cursor;

        std::vector<Module *> m_modules;

        unsigned int m_frames;
        unsigned int m_renderFreq;
        bool m_fullScreen;
    };
}

#include <Genode/System/Application.inl>
#endif