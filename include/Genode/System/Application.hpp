#ifndef GENODE_SYSTEM_APPLICATION_HPP
#define GENODE_SYSTEM_APPLICATION_HPP

#include <Genode/System/Config.hpp>

#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/Graphics/Cursor.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Audio/Mixer.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <functional>
#include <memory>

namespace Gx
{
    class Scene;
    class Module;
    class Application : NonCopyable
    {
    public:
        const std::string TITLE = "O2-JAM";
        static Application &Instance();

        explicit Application(sf::VideoMode mode, bool fullScreen = false);
        Application(sf::VideoMode mode, sf::VideoMode virtualMode, bool fullScreen = false);
        virtual ~Application() = default;

        int Start();
        int Start(Scene &scene);
        void Close();

        SceneDirector &GetSceneDirector() const;

        unsigned int GetRenderFrequency() const;
        void SetCursor(Cursor &cursor);

        template<typename T>
        T &GetConfig();

        template<typename T>
        void SetConfig(const T& config);

        template<typename T>
        void SetConfig(std::function<std::unique_ptr<T>(const Application&)> builder);

        template<typename T>
        T &Install();

        template<typename T>
        bool Provide(std::function<std::unique_ptr<T>(Application&)> builder);

        template<typename T>
        bool Uninstall();

        template<typename T>
        T &Require();

        operator sf::RenderTarget&() const;

    protected:
        sf::RenderWindow &GetRenderWindow() const;

        virtual void Boot();
        virtual void Shutdown();
        virtual void OnFocusChanged(bool focus);
        virtual void OnResized(sf::Event::SizeEvent ev);
        virtual void OnInputReceived(sf::Event ev);
        virtual void OnClose();

    private:
        using ConfigMap         = std::unordered_map<std::type_index, std::unique_ptr<Config>>;
        using ConfigResolverMap = std::unordered_map<std::type_index, std::function<std::unique_ptr<Config>(const Application&)>>;

        using ModuleMap        = std::unordered_map<std::type_index, std::unique_ptr<Module>>;
        using ModuleFactoryMap = std::unordered_map<std::type_index, std::function<std::unique_ptr<Module>(Application&)>>;

        inline static Application *m_instance = nullptr;

        mutable sf::RenderWindow m_window;
        mutable SceneDirector m_director;
        sf::Event m_event;

        ResourceManager m_resources;

        sf::VideoMode m_mode;
        sf::VideoMode m_virtualMode;

        sf::Clock m_timer;
        Cursor* m_cursor;

        ConfigMap         m_configs;
        ConfigResolverMap m_configurators;

        ModuleMap        m_modules;
        ModuleFactoryMap m_factories;

        unsigned int m_frames;
        unsigned int m_renderFreq;
        bool m_fullScreen;
        bool m_closeRequested;
    };
}

#include <Genode/System/Application.inl>
#endif