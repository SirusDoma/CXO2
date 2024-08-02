#ifndef GENODE_AUDIO_MIXER_HPP
#define GENODE_AUDIO_MIXER_HPP

#include <SFML/Audio.hpp>

#include <Genode/System/Provider.hpp>
#include <Genode/Entities/Updatable.hpp>
#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <Genode/IO/ResourceManager.hpp>

#include <vector>
#include <memory>

namespace Gx
{
    class SoundGroup;
    class Mixer : public Provider, public Updatable
    {
    public:
        Mixer();
        Mixer(Mixer&& right) noexcept;
        ~Mixer() override = default;

        Mixer &operator=(Mixer&& right) noexcept;

        SoundGroup *GetMasterSoundGroup() const;
        SoundGroup *GetSoundGroup(const std::string &groupName);

        sf::SoundSource *Play(sf::SoundSource *source);
        sf::SoundSource *Play(sf::SoundSource *source, const std::string &group);
        sf::SoundSource *Play(sf::SoundSource *source, SoundGroup *group);

        void Play(const std::string &groupName);
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
        void Clear();

        void SetVolume(float volume);
        void SetPan(float pan);

        void Update(double delta) override;

    private:
        using SoundGroupContainer  = std::map<std::string, ResourcePtr<SoundGroup>>;
        using SoundSourceContainer = std::vector<ResourcePtr<sf::SoundSource>>;

        ResourcePtr<SoundGroup> m_masterGroup;
        SoundGroupContainer     m_groups;

        SoundSourceContainer m_sources;
    };
}

#include <Genode/Audio/Mixer.inl>
#endif
