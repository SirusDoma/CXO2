#ifndef GENODE_SYSTEM_APPLICATION_HPP
#define GENODE_SYSTEM_APPLICATION_HPP

#include <Genode/Audio/Mixer.hpp>
#include <Genode/Graphics/RenderSurface.hpp>
#include <Genode/Graphics/RenderTargetAdapter.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>
#include <Genode/System/Context.hpp>

#include <SFML/Window.hpp>

#include <functional>
#include <memory>

namespace Gx
{
    class Scene;
    class Cursor;
    class Application : NonCopyable, public Renderable, public Updatable
    {
    public:
        static Application& Instance();

        explicit Application(const std::string& title, const sf::VideoMode& mode, bool fullScreen = false);
        Application(const std::string& title, const sf::VideoMode& mode, const sf::VideoMode& gameVideoMode, bool fullScreen = false);

        ~Application() override = default;

        int Start();
        int Start(Scene& scene);
        void Close();

        Context& GetContext();

        SceneDirector& GetSceneDirector() const;
        unsigned int GetRenderFrequency() const;

        sf::State GetWindowState() const;
        void SetWindowState(const sf::State state);

        const sf::Color& GetClearColor() const;
        void SetClearColor(const sf::Color& clearColor);

        Cursor* GetCursor() const;
        void SetCursor(Cursor& cursor);
        void InvalidateCursor() const;

        sf::View GetVirtualView() const;
        static sf::VideoMode GetDesktopVideoMode();

        // ReSharper disable CppNonExplicitConversionOperator
        operator sf::RenderTarget&() const;
        operator sf::RenderWindow&() const;
        operator RenderSurface&() const;
        // ReSharper restore CppNonExplicitConversionOperator

    protected:
        sf::RenderWindow& GetRenderWindow() const;

        virtual void Boot();
        virtual int Shutdown();

        void Update(double delta) override;
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

        virtual void OnFocusChanged(bool focus);
        virtual void OnResized(sf::Event::SizeEvent ev);
        virtual void OnInputReceived(sf::Event& ev);
        virtual void OnClose();

    private:
        void UpdateCursor(const sf::Event &ev) const;
        void SetupWindow() const;
        void SetupTarget() const;
        static sf::View GetLetterBoxView(sf::View view, sf::Vector2u size);

        inline static Application *m_instance = nullptr;

        mutable std::unique_ptr<sf::RenderWindow> m_window;
        mutable std::unique_ptr<sf::RenderTexture> m_target;
        mutable RenderTargetAdapter m_adapter;
        mutable SceneDirector m_director;

        Context m_context;
        sf::Event m_event;
        sf::State m_state;

        sf::VideoMode m_windowVideoMode;
        sf::VideoMode m_gameVideoMode;
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