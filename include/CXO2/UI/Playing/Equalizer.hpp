#pragma once

#include <CXO2/Core/Chart.hpp>

#include <Genode/Entities/Renderable.hpp>
#include <Genode/Entities/Updatable.hpp>

#include <Genode/UI/UiContainer.hpp>
#include <Genode/UI/Gauge.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include <complex>
#include <cstdint>

namespace Cx
{
    class Equalizer : public virtual Gx::UiContainer
    {
    public:
        // Constructor with configurable bar size and number of bars
        explicit Equalizer(unsigned int sampleBufferCount = 1024);

        // Add a sound to the equalizer
        void Register(const Chart::NoteEvent& ev, const sf::Sound& sound);

        void Update(const sf::Time& delta) override;

    private:
        void Initialize() override;

        // Compute the FFT of the combined audio samples
        void AnalyzeSamples(const std::vector<std::int16_t>& samples, std::vector<float>& output) const;

        std::unordered_map<const Chart::NoteEvent*, const sf::Sound*> m_sounds;    // List of sounds to visualize
        std::vector<Gx::Gauge*> m_bars;                                            // Visual bars

        std::vector<float> m_currents;                                             // Current heights of the bars
        std::vector<float> m_values;                                               // The bar value after FFT and processing
        std::size_t m_bufferSampleCount;
        float m_elapsed;
    };
}
