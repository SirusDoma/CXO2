#pragma once

#include <CXO2/UI/RadioButton.hpp>
#include <CXO2/UI/Image.hpp>
#include <CXO2/UI/BitmapNumber.hpp>
#include <CXO2/UI/UiContainer.hpp>

#include <CXO2/Models/Planet.hpp>

namespace Cx
{
    class ChannelButton : public Cx::RadioButton
    {
    public:
        enum class Mode : std::uint8_t
        {
            Beginner     = 0,
            Intermediate = 1,
            High         = 2
        };

        ChannelButton();
        void Initialize() override;

        sf::FloatRect GetLocalBounds() const override;

        unsigned int GetChannelNumber() const;
        void SetChannelNumber(unsigned int channelNumber);

        unsigned int GetChannelPopulation() const;
        void SetChannelPopulation(unsigned int population, unsigned int maxPopulation);

        MusicHall GetPlanet() const;
        void SetMusicHall(MusicHall hall);

        void AddStateFrame(Mode mode, State state, const sf::IntRect& frame);

    private:
        using StateMap = std::unordered_map<Mode, std::unordered_map<Cx::RadioButton::State, sf::IntRect>>;

        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;
        void OnControlStateChanged(Control& sender, State state) override;

        Image* m_nameIndicator, *m_fullIndicator, *m_focusIndicator;
        BitmapNumber* m_numberIndicator;
        Cx::UiContainer* m_populationCounter;

        StateMap m_states;
        MusicHall m_hall;
        unsigned int m_population, m_maxPopulation;
        bool m_initialized;
    };
}
