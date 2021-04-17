#ifndef GENODE_AUDIO_MIXER_HPP
#define GENODE_AUDIO_MIXER_HPP

#include <SFML/Audio.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/System/Module.hpp>

#include <vector>
#include <memory>
#include <unordered_map>

namespace Gx
{
    class SoundGroup;
    using SoundGroupContainer  = std::unordered_map<std::string, ResourcePtr<SoundGroup>>;

    class Mixer : public Module
    {
    public:
        friend class SceneDirector;

        Mixer();
        virtual ~Mixer();

        sf::SoundSource* Play(sf::SoundSource *source, const std::string &group = "default");
        SoundGroup *GetGroup(const std::string &name);

        void Pause(const std::string &group);
        void Stop(const std::string &group);
        void StopAll();

        virtual void Update(double delta);

    private:
        SoundGroupContainer  m_groups;
    };
}

#endif
