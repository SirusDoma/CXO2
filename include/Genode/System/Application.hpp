#ifndef GENODE_SYSTEM_APPLICATION_HPP
#define GENODE_SYSTEM_APPLICATION_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <Genode/System/Module.hpp>
#include <Genode/Graphics/Cursor.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Audio/Mixer.hpp>

#include <vector>
#include <functional>
#include <memory>

namespace Gx
{
    class Scene;
    class SceneDirector;
    class Config;
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
        void SetCursor(const Cursor &cursor);

        template<typename T>
        T &GetConfig();

        template<typename T>
        void SetConfigResolver(std::function<std::unique_ptr<T>(const Application&)> resolver);

        template<typename T>
        T *Install();

        template<typename T>
        T *Install(T *instance);

        template<typename T>
        bool Uninstall();

        template<typename T>
        T *GetModule() const;

        operator sf::RenderTarget&() const;

    protected:
        sf::RenderWindow &GetRenderWindow() const;

        virtual void OnStart();
        virtual void OnFocusChanged(bool focus);
        virtual void OnResized(sf::Event::SizeEvent ev);
        virtual void OnInputReceived(sf::Event ev);
        virtual void OnClose();

        void ShareResources(ResourceManager &resources);
        void UseMixer(Mixer &mixer);

    private:
        using ConfigMap         = std::unordered_map<std::type_index, std::unique_ptr<Config>>;
        using ConfigResolverMap = std::unordered_map<std::type_index, std::function<std::unique_ptr<Config>(const Application&)>>;

        mutable sf::RenderWindow m_window;
        sf::Event m_event;

        SceneDirector  *m_director;
        ResourceManager m_resources;
        Mixer           m_mixer;

        sf::VideoMode m_mode;
        sf::VideoMode m_virtualMode;

        sf::Clock m_timer;
        Cursor m_cursor;

        ConfigMap         m_configs;
        ConfigResolverMap m_configResolvers;

        std::vector<std::unique_ptr<Module>> m_modules;

        unsigned int m_frames;
        unsigned int m_renderFreq;
        bool m_fullScreen;
    };
}

#include <Genode/System/Application.inl>
#endif