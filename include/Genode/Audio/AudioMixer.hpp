#pragma once

#include <Genode/IO/Resource.hpp>

#include <SFML/Audio.hpp>

#include <unordered_map>
#include <memory>

namespace Gx
{
    class SoundGroup;
    class AudioMixer
    {
    public:
        AudioMixer();

        SoundGroup& GetMasterSoundGroup() const;
        SoundGroup& GetSoundGroup(const std::string& groupName);

        sf::SoundSource& Play(sf::SoundSource& source);
        sf::SoundSource& Play(sf::SoundSource& source, const std::string& group);
        sf::SoundSource& Play(sf::SoundSource& source, SoundGroup& group);

        void Play(const std::string& group);
        void Pause(const std::string& group);
        void Resume(const std::string& group);
        void Stop(const std::string& group);

        void PlayAll();
        void ResumeAll();
        void PauseAll();
        void StopAll();

        void SetVolume(float volume);
        void SetPan(float pan);

        void Reset(bool stop = false);

    private:
        using SoundGroupContainer  = std::unordered_map<std::string, ResourcePtr<SoundGroup>>;

        ResourcePtr<SoundGroup> m_masterGroup;
        SoundGroupContainer     m_groups;
    };
}
