#pragma once

#include <Genode/System/Application.hpp>

class O2Jam : public Gx::Application
{
public:
    using Gx::Application::Application;

    operator sf::RenderTarget&() const override;

protected:
    void Boot() override;
    int  Shutdown() override;
    void OnFocusChanged(bool focus) override;

    void OnInputReceived(sf::Event& ev) override;

    void Update(const double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

private:
    void SetupLayeredTarget() const;
    static sf::View GetLetterBoxView(sf::View view, const sf::Vector2u& windowSize);

    bool m_windowStateSwitched;
    bool m_letterboxSwitched;

    mutable std::unique_ptr<sf::RenderTexture> m_layeredTarget = std::make_unique<sf::RenderTexture>();
    mutable Gx::RenderSurfaceAdaptor m_layeredAdaptor = Gx::RenderSurfaceAdaptor(*m_layeredTarget);
};
