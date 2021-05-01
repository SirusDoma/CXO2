#ifndef O2JAM_PLANET_CHANNEL_BUTTON_HPP
#define O2JAM_PLANET_CHANNEL_BUTTON_HPP

#include <Genode/Graphics/Sprite.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/Number.hpp>
#include <Genode/UI/ProgressBar.hpp>

#include <O2/Data/Planet.hpp>

class ChannelButton : public Gx::RadioButton
{
public:
    ChannelButton();
    virtual const sf::FloatRect GetLocalBounds() const;

    int GetChannelNumber() const;
    void SetChannelNumber(int channelNumber);

    int GetChannelPopulation() const;
    void SetChannelPopulation(int population);

    Planet::MusicHall GetPlanet() const;
    void SetPlanet(Planet::MusicHall hall);

    void SetDefaultMetadata(const Gx::ResourceMetadata *metadata);
    void SetHighMetadata(const Gx::ResourceMetadata *highMetadata);
    void SetIntermediateMetadata(const Gx::ResourceMetadata *intermediateMetadata);
    void SetBeginnerMetadata(const Gx::ResourceMetadata *beginnerMetadata);

private:
    virtual void Initialize();

    virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual void OnControlStateChanged(Control *sender, State state);

    virtual void Invalidate();

    Gx::ResourcePtr<Gx::Image>       m_hover, m_channelName, m_channelFull;
    Gx::ResourcePtr<Gx::Number>      m_channelNumber;
    Gx::ResourcePtr<Gx::ProgressBar> m_channelCounter;
    const Gx::ResourceMetadata *m_defaultMetadata, *m_highMetadata, *m_intermediateMetadata, *m_beginnerMetadata;

    Planet::MusicHall m_hall;
    int m_population;
};

#endif
