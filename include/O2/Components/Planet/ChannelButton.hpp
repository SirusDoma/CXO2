#ifndef O2JAM_PLANET_CHANNEL_BUTTON_HPP
#define O2JAM_PLANET_CHANNEL_BUTTON_HPP

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/Number.hpp>
#include <Genode/UI/ProgressBar.hpp>

#include <O2/Data/Planet.hpp>

class ChannelButton : public Gx::RadioButton
{
public:
    ChannelButton(Gx::Scene &scene, const Gx::RadioButton &copy);
    virtual ~ChannelButton();

    virtual const sf::FloatRect GetLocalBounds() const;

    int GetChannelNumber() const;
    void SetChannelNumber(int channelNumber);

    int GetChannelPopulation() const;
    void SetChannelPopulation(int population);

    Planet GetPlanet() const;
    void SetPlanet(Planet planet);

protected:
    virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual void OnControlStateChanged(Control *sender, State state);

    virtual void Invalidate();

private:
    Gx::Scene       *m_scene;
    Gx::Image       *m_selector, *m_channelName, *m_channelFull;
    Gx::Number      *m_channelNumber;
    Gx::ProgressBar *m_channelCounter;

    Planet m_planet;
};

#endif
