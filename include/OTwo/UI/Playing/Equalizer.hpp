#pragma once

#include <Genode/Entities/Renderable.hpp>
#include <Genode/Entities/Updatable.hpp>

#include <Genode/UI/UiContainer.hpp>
#include <Genode/UI/Gauge.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <kissfft/kiss_fft.h>

#include <vector>
#include <complex>
#include <cstdint>

class Equalizer : public virtual Gx::UiContainer
{
public:
    // Constructor with configurable bar size and number of bars
    explicit Equalizer(unsigned int sampleBufferCount = 2048);

    // Add a sound to the equalizer
    void Register(const sf::Sound& sound);

    // Remove a sound from the equalizer
    void Remove(const sf::Sound& sound);

    void Update(double delta) override;

private:
    // Compute the FFT of the combined audio samples
    void AnalyzeSamples(const std::vector<std::int16_t>& samples, std::vector<float>& magnitudes);

    kiss_fft_cfg kissCfg;
    std::vector<const sf::Sound*> m_sounds;    // List of sounds to visualize
    std::vector<Gx::Gauge*> m_bars;            // Visual bars

    std::vector<float> m_currents;             // Current heights of the bars
    std::vector<float> m_targets;              // Target heights after FFT
    float m_smoothingFactor;                   // For exponential moving average

    std::size_t m_bufferSampleCount;
};
