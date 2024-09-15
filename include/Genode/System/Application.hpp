#ifndef GENODE_SYSTEM_APPLICATION_HPP
#define GENODE_SYSTEM_APPLICATION_HPP

#include <Genode/Audio/Mixer.hpp>
#include <Genode/Graphics/RenderSurface.hpp>
#include <Genode/Graphics/RenderSurfaceAdaptor.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>

#include <SFML/Window.hpp>

#include <functional>
#include <memory>

namespace Gx
{
    class Scene;
    class Cursor;
    class Context;
    class Application : protected Renderable, protected Updatable
    {
    public:
        static Application& Instance();

        Application(const std::string& title, const sf::VideoMode& mode, bool fullScreen = false, const sf::ContextSettings& settings = {});
        Application(const std::string& title, const sf::VideoMode& mode, const sf::View& view, bool fullScreen = false, const sf::ContextSettings& settings = {});

        ~Application() override = default;

        int Start();
        int Start(Scene& scene);
        void Close();

        Context& GetContext() const;

        SceneDirector& GetSceneDirector() const;
        unsigned int GetRenderFrequency() const;

        sf::State GetWindowState() const;
        void SetWindowState(const sf::State state);

        const sf::Color& GetClearColor() const;
        void SetClearColor(const sf::Color& clearColor);

        Cursor* GetCursor() const;
        void SetCursor(Cursor& cursor);
        void InvalidateCursor() const;

        sf::VideoMode GetCurrentVideoMode() const;

        const sf::View& GetInitialView() const;
        virtual const sf::View& GetView() const;
        virtual void SetView(const sf::View& view);

        // ReSharper disable CppNonExplicitConversionOperator
        virtual operator sf::RenderTarget&() const;
        virtual operator sf::RenderWindow&() const;
        virtual operator RenderSurface&() const;
        // ReSharper restore CppNonExplicitConversionOperator

        static sf::VideoMode GetDesktopVideoMode();

    protected:
        sf::RenderWindow& GetMainWindow() const;
        const sf::ContextSettings& GetSettings() const;

        virtual void Boot();
        virtual int Shutdown();

        void Update(double delta) override;
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

        virtual void OnFocusChanged(bool focus);
        virtual void OnResized(const sf::Event::Resized& ev);
        virtual void OnInputReceived(sf::Event& ev);
        virtual void OnClose();

    private:
        void CreateMainWindow() const;
        void UpdateCursor(const sf::Event& ev) const;

        inline static Application* m_instance = nullptr;

        mutable std::unique_ptr<sf::RenderWindow> m_window;
        mutable RenderSurfaceAdaptor m_adaptor;
        mutable SceneDirector m_director;

        std::unique_ptr<Context> m_context;

        sf::State m_state;
        sf::VideoMode m_mode;
        sf::View m_view;
        sf::ContextSettings m_settings;
        Cursor* m_cursor;

        const std::string m_title;
        unsigned int m_frameID;
        unsigned int m_frames;
        unsigned int m_renderFreq;
        bool m_fullScreen;
        bool m_closeRequested;
        sf::Color m_clearColor = sf::Color::Black;
    };
}

#endif