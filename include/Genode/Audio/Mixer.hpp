#ifndef GENODE_AUDIO_MIXER_HPP
#define GENODE_AUDIO_MIXER_HPP

#include <SFML/Audio.hpp>

#include <Genode/Entities/Updatable.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

namespace Gx
{
    class Mixer
    {
    public:
        Mixer();
        virtual ~Mixer();

        sf::SoundSource *Register(sf::SoundSource *source, const std::string& group = "default");
        void Play(sf::SoundSource *source, const std::string& group = "default");

        void Stop(const std::string &group);

        void StopAll();

    private:
        std::unordered_map<std::string, std::vector<std::shared_ptr<sf::SoundSource>>> m_sources;
    };
}

#endif
