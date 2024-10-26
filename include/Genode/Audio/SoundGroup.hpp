#pragma once

#include <Genode/Entities/Updatable.hpp>
#include <SFML/Audio.hpp>

#include <memory>
#include <vector>
#include <string>

namespace Gx
{
    class SoundGroup : public Updatable
    {
    public:
        explicit SoundGroup(const std::string& name);
        ~SoundGroup() override;

        const std::string& GetName() const;
        void SetName(const std::string& name);

        sf::SoundSource::Status GetStatus() const;

        void Play();
        void Resume();
        void Pause();
        void Stop();
        void Clear();

        float GetVolume() const;
        void SetVolume(float volume);

        float GetPan() const;
        void SetPan(float pan);

        bool IsEnabled() const;
        void SetEnabled(bool enable);

    private:
        friend class Mixer;

        sf::SoundSource* Play(sf::SoundSource* source);
        bool Remove(sf::SoundSource* source);

        void Update(double delta) override;

        std::string m_name;
        float m_volume, m_pan;
        bool m_enabled;

        std::vector<sf::SoundSource*> m_sources;
    };
}
