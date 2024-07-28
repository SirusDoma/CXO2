#ifndef GENODE_SYSTEM_APPLICATION_HPP
#define GENODE_SYSTEM_APPLICATION_HPP

#include <Genode/Audio/Mixer.hpp>
#include <Genode/Graphics/Cursor.hpp>
#include <Genode/Graphics/RenderSurface.hpp>
#include <Genode/Graphics/RenderTargetAdapter.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/System/Config.hpp>
#include <Genode/System/Context.hpp>

#include <SFML/Window.hpp>

#include <functional>
#include <memory>

namespace Gx
{
    class Scene;
    class Context;
    class Application : NonCopyable, public Renderable, public Updatable
    {
    public:
        static Application &Instance();

        explicit Application(const std::string &title, const sf::VideoMode &mode, bool fullScreen = false);
        Application(const std::string &title, const sf::VideoMode &mode, const sf::VideoMode &virtualMode, bool fullScreen = false);

        ~Application() override = default;

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
        T &Provide();

        template<typename T>
        bool Provide(std::function<std::unique_ptr<T>(Application&)> builder);

        template<typename T>
        bool Uninstall();

        template<typename T>
        T &Require();

        operator sf::RenderTarget&() const;
        operator RenderSurface&() const;

    protected:
        sf::RenderWindow &GetRenderWindow() const;

        virtual void Boot();
        virtual void Shutdown();

        void Update(double delta) override;
        RenderStates Render(RenderSurface &surface, RenderStates states) const override;

        virtual void OnFocusChanged(bool focus);
        virtual void OnResized(sf::Event::SizeEvent ev);
        virtual void OnInputReceived(sf::Event ev);
        virtual void OnClose();

    private:
        using ConfigMap         = std::unordered_map<std::type_index, std::unique_ptr<Config>>;
        using ConfigResolverMap = std::unordered_map<std::type_index, std::function<std::unique_ptr<Config>(const Application&)>>;

        using ContextMap        = std::unordered_map<std::type_index, std::unique_ptr<Context>>;
        using ContextFactoryMap = std::unordered_map<std::type_index, std::function<std::unique_ptr<Context>(Application&)>>;

        inline static Application *m_instance = nullptr;

        mutable sf::RenderWindow m_window;
        mutable RenderTargetAdapter m_targetAdapter;
        mutable SceneDirector m_director;
        sf::Event m_event;

        ResourceManager m_resources;

        sf::VideoMode m_mode;
        sf::VideoMode m_virtualMode;

        sf::Clock m_timer;
        Cursor* m_cursor;

        ConfigMap         m_configs;
        ConfigResolverMap m_configurators;

        ContextMap        m_contexts;
        ContextFactoryMap m_factories;

        const std::string m_title;
        unsigned int m_frameID;
        unsigned int m_frames;
        unsigned int m_renderFreq;
        bool m_fullScreen;
        bool m_closeRequested;
    };
}

#include <Genode/System/Application.inl>
#endif