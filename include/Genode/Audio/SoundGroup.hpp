#ifndef GENODE_SOUNDGROUP_HPP
#define GENODE_SOUNDGROUP_HPP

#include <SFML/Audio.hpp>

#include <memory>
#include <vector>
#include <string>

namespace Gx
{
    class SoundGroup
    {
    public:
        SoundGroup();
        SoundGroup(const std::string &name);
        virtual ~SoundGroup();

        const std::string &GetName() const;
        void SetName(const std::string &name);

        sf::SoundSource* Play(sf::SoundSource *source);
        void Play();
        void Pause();
        void Stop();
        bool Remove(sf::SoundSource *source);

        void SetVolume(float volume);
        void SetPan(float pan);

    private:
        std::string m_name;
        std::vector<sf::SoundSource*> m_sources;
    };
}

#endif
