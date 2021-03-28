#ifndef GENODE_AUDIO_MIXER_HPP
#define GENODE_AUDIO_MIXER_HPP

#include <SFML/Audio.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceManager.hpp>

#include <vector>
#include <memory>
#include <unordered_map>

namespace Gx
{
    class SoundGroup;
    using SoundGroupContainer  = std::unordered_map<std::string, ResourcePtr<SoundGroup>>;

    class Mixer
    {
    public:
        friend class SceneDirector;

        Mixer();
        virtual ~Mixer();

        sf::SoundSource* Play(sf::SoundSource *source, const std::string &group = "default");
        void Pause(const std::string &group);
        void Stop(const std::string &group);
        void StopAll();

        SoundGroup *GetGroup(const std::string &name);

    private:
        SoundGroupContainer  m_groups;
    };
}

#endif
