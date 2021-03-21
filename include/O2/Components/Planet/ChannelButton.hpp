#ifndef O2JAM_PLANET_CHANNEL_BUTTON_HPP
#define O2JAM_PLANET_CHANNEL_BUTTON_HPP

#include <Genode/Graphics/Sprite.hpp>

#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/Number.hpp>

#include <O2/Data/Planet.hpp>

class ChannelButton : public Gx::RadioButton
{
public:
    ChannelButton(const Gx::RadioButton &copy);
    virtual ~ChannelButton();

    virtual const sf::FloatRect GetLocalBounds() const;

    int GetChannelNumber() const;
    void SetChannelNumber(int channelNumber);

    Planet GetPlanet() const;
    void SetPlanet(Planet planet);

protected:
    virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual void OnControlStateChanged(Control *sender, State state);

    virtual void Invalidate();

private:
    Gx::Image *m_selector, *m_channelName;
    Gx::Number *m_channelNumber;

    Planet m_planet;
};

#endif
