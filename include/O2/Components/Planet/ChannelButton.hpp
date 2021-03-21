#ifndef O2JAM_PLANET_CHANNEL_BUTTON_HPP
#define O2JAM_PLANET_CHANNEL_BUTTON_HPP

#include <Genode/Graphics/Sprite.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/Image.hpp>

#include <O2/Data/Planet.hpp>

class ChannelButton : public Gx::RadioButton
{
public:
    ChannelButton(const Gx::RadioButton &copy);
    virtual ~ChannelButton();

    virtual const sf::FloatRect GetLocalBounds() const;
    virtual void SetPlanet(Planet planet);
    
protected:
    virtual void OnControlStateChanged(Control *sender, State state);
    virtual void Invalidate();

private:
    Gx::Image *m_selector, *m_channelName;
};

#endif
