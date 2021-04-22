#ifndef GENODE_AUDIO_MIXER_HPP
#define GENODE_AUDIO_MIXER_HPP

#include <SFML/Audio.hpp>

#include <Genode/System/Module.hpp>
#include <Genode/Entities/Updatable.hpp>
#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceManager.hpp>

#include <vector>
#include <memory>
#include <unordered_map>

namespace Gx
{
    class SoundGroup;
    class Mixer : public Module, public Updatable
    {
    public:
        Mixer();
        Mixer(Mixer &&other);
        Mixer(Gx::ResourceManager &sharedResource);
        virtual ~Mixer();

        template<typename R>
        R *Create(const std::string &source);

        SoundGroup *GetMasterSoundGroup() const;
        SoundGroup *GetSoundGroup(const std::string &name) const;

        sf::SoundSource *Play(sf::SoundSource *source);
        sf::SoundSource *Play(sf::SoundSource *source, const std::string &group);
        sf::SoundSource *Play(sf::SoundSource *source, SoundGroup *group);

        void Play(const std::string &group);
        void Play(SoundGroup *group);

        void Pause(sf::SoundSource *source);
        void Pause(const std::string &group);
        void Pause(SoundGroup *group);

        void Resume(sf::SoundSource *source);
        void Resume(const std::string &group);
        void Resume(SoundGroup *group);

        void Stop(sf::SoundSource *source);
        void Stop(const std::string &group);
        void Stop(SoundGroup *group);

        void PlayAll();
        void ResumeAll();
        void PauseAll();
        void StopAll();

        void SetVolume(float volume);
        void SetPan(float pan);


        virtual void Update(double delta);

        Mixer &operator =(Mixer&& right);

    private:
        using SoundGroupContainer  = std::map<std::string, std::unique_ptr<SoundGroup>>;
        using SoundSourceContainer = std::vector<ResourcePtr<sf::SoundSource>>;

        std::unique_ptr<SoundGroup> m_masterGroup;
        SoundGroupContainer         m_groups;

        SoundSourceContainer m_sources;
        Gx::ResourceManager *m_resources;
    };
}

#include <Genode/Audio/Mixer.inl>
#endif
