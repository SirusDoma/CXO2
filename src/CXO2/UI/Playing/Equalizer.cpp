#include <CXO2/UI/Playing/Equalizer.hpp>
#include <CXO2/Constants/Identifiers/Game.hpp>

#include <Genode/UI/List.hpp>

#include <dj_fft/dj_fft.h>

#include <algorithm>
#include <cmath>

namespace Cx
{
    using namespace Constants::Identifiers;

    Equalizer::Equalizer(const unsigned int sampleBufferCount) :
        m_bufferSampleCount(sampleBufferCount),
        m_elapsed(0.f)
    {
    }

    void Equalizer::Initialize()
    {
        UiContainer::Initialize();

        if (m_bars.empty())
        {
            const auto list = FindChild<Gx::List>(Resource::Game::Equalizer::IDC_LIST_BAR);
            if (!list)
                return;

            const auto bars = list->GetChildren();
            for (const auto child : bars)
            {
                if (auto bar = dynamic_cast<Gx::Gauge*>(child))
                {
                    m_bars.push_back(bar);
                    m_values.push_back(0.0f);
                }
            }
        }
    }

    void Equalizer::Register(const Chart::NoteEvent& ev, const sf::Sound& sound)
    {
        m_sounds[&ev] = &sound;
    }

    // Update visualization
    void Equalizer::Update(const sf::Time& delta)
    {
        if (m_bars.empty())
            Initialize();

        if (m_bars.empty())
            return;

        for(auto it = std::begin(m_sounds); it != std::end(m_sounds);)
        {
            if (it->second->getStatus() != sf::SoundSource::Status::Playing)
                it = m_sounds.erase(it);
            else
                ++it;
        }

        // Combined samples from all sounds
        std::vector<std::int16_t> combinedSamples;
        std::unordered_set<const sf::SoundBuffer*> buffers;
        std::unordered_set<long> timestamps;
        for (const auto& [_, sound] : m_sounds)
        {
            if (sound->getStatus() == sf::Sound::Status::Playing)
            {
                const sf::SoundBuffer& buffer = sound->getBuffer();
                sf::Time offset = sound->getPlayingOffset();
                if (offset == sf::Time::Zero)
                    continue;

                if (auto [_, success] = buffers.insert(&buffer); !success)
                {
                    if (auto [_, success] = timestamps.insert(offset.asMilliseconds()); !success)
                        continue;
                }

                const std::int16_t* audioSamples = buffer.getSamples();

                // Calculate sample index based on offset
                auto samples = std::vector<std::int16_t>(m_bufferSampleCount, 0);
                std::size_t position = static_cast<std::size_t>(offset.asSeconds() * buffer.getSampleRate() * buffer.getChannelCount());

                // Re-align start position due to floating math. Assuming LRLRLR format; The left channel indices are always even.
                position -= buffer.getChannelCount() >= 2 && position % 2 != 0 ? 1 : 0;
                for (std::size_t i = 0; i < m_bufferSampleCount; ++i)
                {
                    samples[i] = audioSamples[position];
                    position  += buffer.getChannelCount();

                    if (position >= buffer.getSampleCount())
                        break;
                }

                // Mix samples
                if (!combinedSamples.empty())
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
                else
                    combinedSamples = samples;
            }
        }

        // If there are samples, compute FFT and update target heights
        if (!combinedSamples.empty())
        {
            auto magnitudes = std::vector<float>();
            std::size_t paddedSize = 1;
            while (paddedSize < combinedSamples.size())
                paddedSize <<= 1;

            combinedSamples.resize(paddedSize, 0);

            AnalyzeSamples(combinedSamples, magnitudes);
            if (magnitudes.empty())
                return;

            for (std::size_t i = 0; i < m_bars.size(); ++i)
                m_values[i] = magnitudes[i];
        }
        else
        {
            // No sounds playing, target heights should be zero
            for (auto& value: m_values)
                value = 0.0f;
        }

        for (std::size_t i = 0; i < m_bars.size(); ++i)
            m_bars[i]->SetValue(std::max(1.f, 1.f + m_values[i]));

        m_elapsed = 0.f;
    }

    void Equalizer::AnalyzeSamples(const std::vector<std::int16_t>& samples, std::vector<float>& output) const
    {
        const std::size_t N = samples.size();

        std::vector<std::complex<float>> in(N);

        constexpr float normalizeFactor = 32768.f; // SHORT_MAX + 1.f
        for (std::size_t i = 0; i < N; ++i)
            in[i] = { static_cast<float>(samples[i]) / normalizeFactor, 0.f };

        const auto out = dj::fft1d(in, dj::fft_dir::DIR_FWD);

        constexpr std::array<std::size_t, 23> freqIndices = {
            2,
            5,
            8,
            12,
            15,
            19,
            22,
            26,
            30,
            35,
            39,
            44,
            49,
            53,
            58,
            65,
            72,
            79,
            111,
            144,
            190,
            237,
            279
        };

        const float unitaryFactor = std::sqrt(static_cast<float>(N));

        auto magnitudes = std::vector<float>(N);
        for (std::size_t i = 0; i < N; i++)
        {
            constexpr float scaleFactor = 32.f;
            const float magnitude       = std::abs(out[i]) * unitaryFactor / scaleFactor;

            magnitudes[i] += magnitude;
        }

        auto peaks = std::vector(freqIndices.size(), 0.f);
        output.assign(peaks.size(), 0.f);

        std::size_t j = 0;
        for (std::size_t i = 0; i < freqIndices.size(); i++)
        {
            constexpr float maxPeak     = 8.0f;
            constexpr float scaleFactor = 4.0f;

            while (j < freqIndices[i])
            {
                const float magnitude = magnitudes[j];
                if (magnitude > peaks[i])
                    peaks[i] = magnitude;

                j++;
            }

            peaks[i] = std::min(peaks[i] * scaleFactor, maxPeak);
            output[i] = floor(peaks[i]);
        }
    }
}
