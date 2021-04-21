#ifndef GENODE_AUDIO_SOUNDGROUP_HPP
#define GENODE_AUDIO_SOUNDGROUP_HPP

#include <SFML/Audio.hpp>

#include <memory>
#include <vector>
#include <string>

namespace Gx
{
    class SoundGroup
    {
    public:
        ~SoundGroup();

        const std::string &GetName() const;
        void SetName(const std::string &name);

        void Resume();
        void Pause();
        void Stop();

        float GetVolume() const;
        void SetVolume(float volume);

        float GetPan() const;
        void SetPan(float pan);

    private:
        friend class Mixer;
        SoundGroup(const std::string &name);

        sf::SoundSource* Play(sf::SoundSource *source);
        bool Remove(sf::SoundSource *source);

        std::string m_name;
        float m_volume, m_pan;

        std::vector<sf::SoundSource*> m_sources;
    };
}

#endif
