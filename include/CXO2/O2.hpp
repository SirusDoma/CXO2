#pragma once

#include <Genode/System/Application.hpp>

namespace Cx
{
    enum class InteropMode : int
    {
        None      = 0,
        Interface = 1 << 0,
        Playing   = 1 << 1,
        Avatar    = 1 << 2
    };

    inline InteropMode operator|(InteropMode a, InteropMode b)
    {
        return static_cast<InteropMode>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline bool operator&(InteropMode a, InteropMode b)
    {
        return (static_cast<int>(a) & static_cast<int>(b)) != 0;
    }

    class O2 : public Gx::Application
    {
    public:
        inline static std::string Version = "3.10";

        O2(std::string title, const sf::VideoMode& mode, const sf::View& view, bool fullScreen = false, const sf::ContextSettings& settings = {});

        static bool InInteropMode();
        static bool InInteropMode(InteropMode modes);

        // ReSharper disable once CppNonExplicitConversionOperator
        operator sf::RenderTarget&() const override;

    protected:
        void Boot() override;
        int  Shutdown() override;

        void OnWindowCreated(sf::RenderWindow& window) override;
        void OnFocusChanged(bool focus) override;
        void OnInputReceived(sf::Event& ev) override;

        sf::VideoMode GetVideoMode() const override;

        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

    private:
        void SetupLayeredTarget() const;
        static sf::View GetLetterBoxView(sf::View view, const sf::Vector2u& windowSize);

        std::optional<sf::Image> m_icon{std::nullopt};
        bool m_windowStateSwitched{false};
        bool m_letterboxSwitched{false};

        mutable std::unique_ptr<sf::RenderTexture> m_layeredTarget = std::make_unique<sf::RenderTexture>();
        mutable Gx::RenderSurfaceAdaptor m_layeredAdaptor = Gx::RenderSurfaceAdaptor(*m_layeredTarget);
    };
}
