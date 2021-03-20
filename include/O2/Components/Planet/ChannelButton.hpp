#ifndef O2JAM_PLANET_CHANNEL_BUTTON_HPP
#define O2JAM_PLANET_CHANNEL_BUTTON_HPP

#include <Genode/Graphics/Sprite.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/Image.hpp>

#include <O2/Data/Planet.hpp>

class ChannelButton : public Gx::Control
{
public:
    ChannelButton();
    virtual const sf::FloatRect GetLocalBounds() const;

    virtual void SetPlanet(Planet planet);

protected:
    virtual void OnControlStateChanged(Control *sender, State state);
    virtual void Invalidate();

private:
    void Initialize();

    Gx::Button *m_button;
    Gx::Image  *m_selector;
};

#endif
