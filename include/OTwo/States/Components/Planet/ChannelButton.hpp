#ifndef O2JAM_PLANET_CHANNEL_BUTTON_HPP
#define O2JAM_PLANET_CHANNEL_BUTTON_HPP

#include <Genode/Graphics/Sprite.hpp>

#include <Genode/IO/Resource.hpp>
#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/Number.hpp>
#include <Genode/UI/ProgressBar.hpp>

#include <OTwo/Data/Planet.hpp>

class ChannelButton : public Gx::RadioButton
{
public:
    enum Mode
    {
        Beginner,
        Intermediate,
        High
    };

    ChannelButton();
    void Initialize() override;

    const sf::FloatRect GetLocalBounds() const override;

    unsigned int GetChannelNumber() const;
    void SetChannelNumber(unsigned int channelNumber);

    unsigned int GetChannelPopulation() const;
    void SetChannelPopulation(unsigned int population);

    MusicHall GetPlanet() const;
    void SetMusicHall(MusicHall hall);

    void AddStateFrame(Mode mode, State state, const sf::IntRect &frame);

private:
    using StateMap = std::unordered_map<Mode, std::unordered_map<Gx::RadioButton::State, sf::IntRect>>;

    sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const override;
    void OnControlStateChanged(Control *sender, State state) override;
    void Invalidate() override;

    Gx::Image       *m_nameIndicator, *m_fullIndicator, *m_focusIndicator;
    Gx::Number      *m_numberIndicator;
    Gx::ProgressBar *m_populationCounter;

    StateMap m_states;
    MusicHall m_hall;
    unsigned int m_population;
    bool m_initialized;
};

#endif
