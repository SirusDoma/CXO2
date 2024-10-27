#include <OTwo/UI/Playing/Equalizer.hpp>

#include <Genode/UI/List.hpp>

#include <kissfft/kiss_fft.c>
#include <kissfft/kiss_fftr.c>

#include <algorithm>
#include <cmath>

Equalizer::Equalizer(const unsigned int sampleBufferCount) :
    m_smoothingFactor(0.2f),
    m_bufferSampleCount(sampleBufferCount)
{
    kissCfg = kiss_fft_alloc(m_bufferSampleCount, 0, nullptr, nullptr);
}

// Add a sound to visualize
void Equalizer::Register(const sf::Sound& sound)
{
    m_sounds.push_back(&sound);
}

// Remove a sound from visualization
void Equalizer::Remove(const sf::Sound& sound)
{
    m_sounds.erase(std::remove(m_sounds.begin(), m_sounds.end(), &sound), m_sounds.end());
}

// Update visualization
void Equalizer::Update(double delta)
{
    if (m_bars.empty())
    {
        const auto list = FindChild<Gx::List>("IDC_LIST_BAR");
        if (!list)
            return;

        const auto bars = list->GetChildren();
        for (const auto child : bars)
        {
            if (auto bar = dynamic_cast<Gx::Gauge*>(child))
            {
                m_bars.push_back(bar);
                m_currents.push_back(0.0f);
                m_targets.push_back(0.0f);
            }
        }

        if (m_bars.empty())
            return;
    }

    m_sounds.erase(
        std::remove_if(m_sounds.begin(), m_sounds.end(), [=] (const sf::Sound* s)
        {
            return s->getStatus() != sf::SoundSource::Status::Playing;
        }),
        m_sounds.end()
    );

    // Combined samples from all sounds
    std::vector<std::int16_t> combinedSamples;
    std::unordered_set<const sf::SoundBuffer*> buffers;
    std::unordered_set<long> timestamps;
    for (const auto& sound: m_sounds)
    {
        if (sound->getStatus() == sf::Sound::Status::Playing)
        {
            const sf::SoundBuffer& buffer = sound->getBuffer();
            sf::Time offset = sound->getPlayingOffset();

            if (auto [_, success] = buffers.insert(&buffer); !success)
            {
                if (auto [_, success] = timestamps.insert(offset.asMilliseconds()); !success)
                    continue;
            }

            const std::int16_t* bufferSamples = buffer.getSamples();
            const std::size_t bufferSampleCount = buffer.getSampleCount();
            const unsigned int bufferChannelCount = buffer.getChannelCount();
            const unsigned int bufferSampleRate = buffer.getSampleRate();

            // Calculate sample index based on offset
            // TODO: Fix intermittent garbage signals at the end of sound
            const std::size_t currentSamplePosition = static_cast<std::size_t>(offset.asSeconds() * bufferSampleRate * bufferChannelCount);
            std::vector<std::int16_t> samples;
            for (std::size_t i = 0; i < m_bufferSampleCount && currentSamplePosition + i < bufferSampleCount; ++i)
                samples.push_back(bufferSamples[currentSamplePosition + i]);

            // Mix samples
            if (combinedSamples.empty())
            {
                combinedSamples = samples;
            }
            else
            {
                for (std::size_t i = 0; i < samples.size(); ++i)
                {
                    if (i < combinedSamples.size())
                    {
                        const auto mixedSample = static_cast<std::int32_t>(combinedSamples[i]) + static_cast<std::int32_t>(samples[i]);
                        combinedSamples[i]     = static_cast<std::int16_t>(std::clamp(mixedSample, -32768, 32767));
                    }
                    else
                        combinedSamples.push_back(samples[i]);
                }
            }
        }
    }

    // If there are samples, compute FFT and update target heights
    if (!combinedSamples.empty())
    {
        auto magnitudes = std::vector<float>(m_bars.size());
        if (combinedSamples.size() % 2 != 0)
            combinedSamples.push_back(0);

        AnalyzeSamples(combinedSamples, magnitudes);

        // Normalize magnitudes
        float maxMagnitude = *std::max_element(magnitudes.begin(), magnitudes.end());
        if (maxMagnitude == 0.0f)
            maxMagnitude = 1.0f; // Prevent division by zero

        // Map magnitudes to target heights
        for (std::size_t i = 0; i < m_bars.size(); ++i)
        {
            m_targets[i] = magnitudes[i] * m_bars[i]->GetMaximumValue();
        }
    }
    else
    {
        // No sounds playing, target heights should be zero
        for (auto& targetHeight: m_targets)
        {
            targetHeight = 0.0f;
        }
    }

    // Smoothly update bar heights using exponential moving average
    for (std::size_t i = 0; i < m_bars.size(); ++i)
    {
        //m_currents[i] = m_currents[i] * (1.0f - m_smoothingFactor) + m_targets[i] * m_smoothingFactor;
        m_currents[i] = std::min(10 + m_targets[i], m_bars[i]->GetMaximumValue());

        // Ensure bar heights do not change too rapidly
        // const float heightDifference = m_currents[i] - m_bars[i]->GetMaximumValue();
        // constexpr float maxChange = 5.0f; // Maximum change per frame
        // if (std::abs(heightDifference) > maxChange)
        // {
        //     if (heightDifference > 0)
        //         m_currents[i] = m_bars[i]->GetValue() + maxChange;
        //     else
        //         m_currents[i] = m_bars[i]->GetValue() - maxChange;
        // }

        // Update bar value
        m_bars[i]->SetValue(m_currents[i]);
    }
}

// (DFT implementation)
// void Equalizer::AnalyzeSamples(const std::vector<std::int16_t>& samples, std::vector<float>& magnitudes)
// {
//     std::size_t N = m_bars.size(); // Number of frequency bins
//     magnitudes.resize(N, 0.0f);
//
//     std::size_t sampleCount = samples.size();
//     for (std::size_t k = 0; k < N; ++k)
//     {
//         std::complex<float> sum(0.0f, 0.0f);
//         for (std::size_t n = 0; n < sampleCount; ++n)
//         {
//             float angle = 2.0f * 3.14159265f * k * n / sampleCount;
//             sum += std::complex<float>(samples[n] * std::cos(angle), -samples[n] * std::sin(angle));
//         }
//         magnitudes[k] = std::abs(sum);
//     }
// }
//
// void Equalizer::AnalyzeSamples(const std::vector<std::int16_t>& samples, std::vector<float>& magnitudes)
// {
//     // PLEASE DON'T QUOTE ME ON THIS, THE CODE TOTALLY GENERATED
//
//     const std::size_t N = samples.size();
//     if (kissCfg->nfft != N)
//     {
//         free(kissCfg);
//         kissCfg = kiss_fft_alloc(N, 0, nullptr, nullptr);
//     }
//
//     std::vector<kiss_fft_cpx> in(N);
//     std::vector<kiss_fft_cpx> out(N);
//
//     // TODO: Cache the input?
//     for (std::size_t i = 0; i < N; ++i)
//     {
//         const float hanningWindow = 0.5f * (1.0f - std::cos(2.0f * 3.14159265f * i / (N - 1)));
//         in[i].r = static_cast<float>(samples[i]) * hanningWindow;
//         in[i].i = 0.0f;
//     }
//
//     kiss_fft(kissCfg, in.data(), out.data());
//
//     const std::size_t binSize = (N / 2 + 1) / magnitudes.size();
//     for (std::size_t i = 0; i < magnitudes.size(); ++i)
//     {
//         double sum = 0.0;
//         for (std::size_t j = i * binSize; j < (i + 1) * binSize; ++j)
//         {
//             const float real = out[j].r;
//             const float imag = out[j].i;
//             sum += std::sqrt(real * real + imag * imag);
//         }
//         magnitudes[i] = static_cast<float>(sum / static_cast<double>(binSize));
//     }
//
// }

void Equalizer::AnalyzeSamples(const std::vector<std::int16_t>& samples, std::vector<float>& magnitudes)
{
    const std::size_t N = samples.size();
    const auto cfg = kiss_fftr_alloc(N, 0, nullptr, nullptr);

    std::vector<float> in(N);
    std::vector<kiss_fft_cpx> out(N / 2 + 1);

    for (std::size_t i = 0; i < N; ++i)
    {
        const float hanningWindow = 0.5f * (1.0f - std::cos(2.0f * 3.14159265f * i / (N - 1)));
        in[i] = std::clamp(static_cast<float>(samples[i]) / std::numeric_limits<int16_t>::max() * hanningWindow, -1.f, 1.f);
    }

    kiss_fftr(cfg, in.data(), out.data());

    const std::vector<float> frequencies = { 20, 250, 500, 2000, 6000, 20000 };
    auto peaks = std::vector<float>();
    for (std::size_t i = 0; i < N / 2 + 1; i++)
    {
        constexpr std::size_t sampleRate   = 44100;
        constexpr std::size_t channelCount = 2;

        const float frequency = i * (channelCount * sampleRate) / N;
        const float magnitude = std::hypot(out[i].r, out[i].i);

        for (std::size_t j = 0; j < frequencies.size() - 1; j++)
        {
            if (frequency > frequencies[j] && frequency <= frequencies[j + 1])
            {
                peaks.push_back(magnitude);
            }
        }
    }

    for (std::size_t i = 0; i < magnitudes.size(); i++)
    {
        if (i >= peaks.size())
            break;

        magnitudes[i] = peaks[i] * 0.013f;
    }

    free(cfg);
}
