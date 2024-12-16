#pragma once

#include <Genode/Graphics/RenderSurface.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>

#include <SFML/Window.hpp>
#include <memory>

namespace Gx
{
    class Scene;
    class Cursor;
    class Context;
    class Application : public RenderSurface, protected Renderable, protected Updatable
    {
    public:
        static Application& Instance();

        ~Application() override = default;

        int Start();
        void Close();

        const std::string& GetTitle() const;
        Context& GetContext() const;

        SceneDirector& GetSceneDirector() const;
        unsigned int GetRenderFrequency() const;

        sf::State GetWindowState() const;
        void SetWindowState(sf::State state);

        const sf::Color& GetClearColor() const;
        void SetClearColor(const sf::Color& clearColor);

        Cursor* GetCursor() const;
        void SetCursor(Cursor& cursor);
        void InvalidateCursor() const;

        sf::VideoMode GetCurrentVideoMode() const;

        const sf::View& GetDefaultView() const override;
        const sf::View& GetView() const override;
        void SetView(const sf::View& view) override;

        void Clear(sf::Color clearColor) override;
        void Clear(sf::Color clearColor, sf::StencilValue stencilValue) override;
        void Render(const Renderable& renderable, const RenderStates& states) override;
        void Render(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type, const RenderStates& states) override;
        void Render(const sf::VertexBuffer& vertexBuffer, const RenderStates& states) override;
        void Render(const sf::VertexBuffer& vertexBuffer, std::size_t firstVertex, std::size_t vertexCount,const RenderStates& states) override;

        // ReSharper disable CppNonExplicitConversionOperator
        virtual operator sf::RenderTarget&() const;
        virtual operator sf::RenderWindow&() const;
        // ReSharper restore CppNonExplicitConversionOperator

        static sf::VideoMode GetDesktopVideoMode();

    protected:
        Application(std::string title, const sf::VideoMode& mode, bool fullScreen = false, const sf::ContextSettings& settings = {});
        Application(std::string title, const sf::VideoMode& mode, const sf::View& view, bool fullScreen = false, const sf::ContextSettings& settings = {});

        sf::RenderWindow& GetMainWindow() const;
        const sf::ContextSettings& GetSettings() const;

        virtual void Boot() = 0;
        virtual int Shutdown() = 0;

        void Update(double delta) override;
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

        virtual void OnWindowCreated(sf::RenderWindow& window);
        virtual void OnFocusChanged(bool focus);
        virtual void OnResized(const sf::Vector2u& size);
        virtual void OnInputReceived(sf::Event& ev);
        virtual bool OnClose();

    private:
        void CreateMainWindow();
        void UpdateCursor(const sf::Event& ev) const;

        inline static Application* m_instance = nullptr;

        mutable std::unique_ptr<sf::RenderWindow> m_window;
        mutable SceneDirector m_director;
        mutable Context m_context;

        sf::State m_state;
        sf::VideoMode m_mode;
        sf::View m_view;
        sf::ContextSettings m_settings;
        Cursor* m_cursor;

        const std::string m_title;
        unsigned int m_frameID;
        unsigned int m_renderFreq;
        bool m_fullScreen;
        bool m_closeRequested;
        sf::Color m_clearColor = sf::Color::Black;
    };
}
