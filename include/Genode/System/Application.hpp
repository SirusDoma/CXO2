#ifndef GENODE_SYSTEM_APPLICATION_HPP
#define GENODE_SYSTEM_APPLICATION_HPP

#include <Genode/Audio/Mixer.hpp>
#include <Genode/Graphics/Cursor.hpp>
#include <Genode/Graphics/RenderSurface.hpp>
#include <Genode/Graphics/RenderTargetAdapter.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/System/Provider.hpp>

#include <SFML/Window.hpp>

#include <functional>
#include <memory>

namespace Gx
{
    class Scene;
    class Provider;
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

        sf::State GetWindowState() const;
        void SetWindowState(const sf::State state);

        const sf::Color& GetClearColor() const;
        void SetClearColor(const sf::Color &clearColor);

        void SetCursor(Cursor &cursor);

        sf::View GetVirtualView() const;

        template<typename T>
        T &Provide();

        template<typename T>
        bool Provide(std::function<std::unique_ptr<T>(Application&)> builder);

        template<typename T>
        bool Uninstall();

        template<typename T>
        T &Require();

        operator sf::RenderTarget&() const;
        operator sf::RenderWindow&() const;
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
        using ProviderMap        = std::unordered_map<std::type_index, std::unique_ptr<Provider>>;
        using ProviderFactoryMap = std::unordered_map<std::type_index, std::function<std::unique_ptr<Provider>(Application&)>>;

        void SetupWindow() const;
        void SetupTarget() const;
        static sf::View GetLetterBoxView(sf::View view, sf::Vector2u size);

        inline static Application *m_instance = nullptr;

        mutable std::unique_ptr<sf::RenderWindow> m_window;
        mutable std::unique_ptr<sf::RenderTexture> m_target;
        mutable RenderTargetAdapter m_adapter;
        mutable SceneDirector m_director;
        sf::Event m_event;
        sf::State m_state;

        sf::VideoMode m_mode;
        sf::VideoMode m_virtualMode;
        Cursor* m_cursor;

        ProviderMap        m_providers;
        ProviderFactoryMap m_factories;

        const std::string m_title;
        unsigned int m_frameID;
        unsigned int m_frames;
        unsigned int m_renderFreq;
        bool m_fullScreen;
        bool m_closeRequested;
        sf::Color m_clearColor = sf::Color::Black;
    };
}

#include <Genode/System/Application.inl>
#endif