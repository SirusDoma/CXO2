#ifndef GENODE_AUDIO_SOUNDGROUP_HPP
#define GENODE_AUDIO_SOUNDGROUP_HPP

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
        ~SoundGroup();

        const std::string &GetName() const;
        void SetName(const std::string &name);

        sf::SoundSource::Status GetStatus() const;

        void Play();
        void Resume();
        void Pause();
        void Stop();

        float GetVolume() const;
        void SetVolume(float volume);

        float GetPan() const;
        void SetPan(float pan);

        bool IsEnabled() const;
        void SetEnabled(bool enable);

    private:
        friend class Mixer;
        SoundGroup(const std::string &name);

        sf::SoundSource* Play(sf::SoundSource *source);
        bool Remove(sf::SoundSource *source);

        virtual void Update(double delta);

        std::string m_name;
        float m_volume, m_pan;
        bool m_enabled;

        std::vector<sf::SoundSource*> m_sources;
    };
}

#endif
