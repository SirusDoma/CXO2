#ifndef O2JAM_HPP
#define O2JAM_HPP

#include <Genode/System/Application.hpp>

class O2Jam : public Gx::Application
{
public:
    using Gx::Application::Application;

    ~O2Jam() override = default;

protected:
    void Boot() override;
    int  Shutdown() override;
    void OnFocusChanged(bool focus) override;

    void OnInputReceived(sf::Event& ev) override;

    void Update(const double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

private:
    bool m_switched;
};

#endif
