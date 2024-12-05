#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/Audio/SoundGroup.hpp>
#include <Genode/System/Exception.hpp>

namespace Gx
{
    AudioMixer::AudioMixer() :
        m_masterGroup(std::make_unique<SoundGroup>("master")),
        m_groups()
    {
    }

    SoundGroup& AudioMixer::GetMasterSoundGroup() const
    {
        return *m_masterGroup.get();
    }

    SoundGroup& AudioMixer::GetSoundGroup(const std::string& groupName)
    {
        if (groupName == m_masterGroup->GetName())
            return *m_masterGroup;

        if (const auto iterator = m_groups.find(groupName); iterator == m_groups.end())
            m_groups[groupName] = std::make_unique<SoundGroup>(groupName);

        return *m_groups[groupName].get();
    }

    sf::SoundSource& AudioMixer::Play(sf::SoundSource& source)
    {
        return Play(source, *m_masterGroup.get());
    }

    sf::SoundSource& AudioMixer::Play(sf::SoundSource& source, const std::string& group)
    {
        if (group.empty() || group == m_masterGroup->GetName())
            return Play(source, *m_masterGroup.get());

        return Play(source, GetSoundGroup(group));
    }

    sf::SoundSource& AudioMixer::Play(sf::SoundSource& source, SoundGroup& group)
    {
        if (&group != m_masterGroup.get())
            m_masterGroup->Remove(source);

        for (auto& [_, grp] : m_groups)
        {
            if (grp.get() != &group)
                grp->Remove(source);
        }

        return group.Play(source);
    }

    void AudioMixer::Play(const std::string& group)
    {
        if (group.empty())
            return;

        if (group == m_masterGroup->GetName())
            m_masterGroup->Play();
        else if (const auto& it = m_groups.find(group); it != m_groups.end())
            it->second->Play();
    }

    void AudioMixer::Pause(const std::string& group)
    {
        if (group.empty())
            return;

        if (group == m_masterGroup->GetName())
            m_masterGroup->Pause();
        else if (const auto& it = m_groups.find(group); it != m_groups.end())
            it->second->Pause();
    }

    void AudioMixer::Resume(const std::string& group)
    {
        if (group.empty())
            return;

        if (group == m_masterGroup->GetName())
            m_masterGroup->Resume();
        else if (const auto& it = m_groups.find(group); it != m_groups.end())
            it->second->Resume();
    }

    void AudioMixer::Stop(const std::string& group)
    {
        if (group.empty())
            return;

        if (group != m_masterGroup->GetName())
        {
            if (const auto& iterator = m_groups.find(group); iterator != m_groups.end())
                iterator->second->Stop();
        }
        else
            m_masterGroup->Stop();
    }

    void AudioMixer::PlayAll()
    {
        m_masterGroup->Play();
        for (auto& [_, group] : m_groups)
            group->Play();
    }

    void AudioMixer::ResumeAll()
    {
        m_masterGroup->Resume();
        for (auto& [_, group] : m_groups)
            group->Resume();
    }

    void AudioMixer::PauseAll()
    {
        m_masterGroup->Pause();
        for (auto& [_, group] : m_groups)
            group->Pause();
    }

    void AudioMixer::StopAll()
    {
        m_masterGroup->Stop();
        for (auto& [_, group] : m_groups)
            group->Stop();
    }

    void AudioMixer::SetVolume(const float volume)
    {
        m_masterGroup->SetVolume(volume);
        for (auto& [_, group] : m_groups)
            group->SetVolume(volume);
    }

    void AudioMixer::SetPan(const float pan)
    {
        m_masterGroup->SetPan(pan);
        for (auto& [_, group] : m_groups)
            group->SetPan(pan);
    }

    void AudioMixer::Reset(const bool stop)
    {
        if (stop)
            m_masterGroup->Stop();

        m_masterGroup->Reset();
        for (auto& [_, group] : m_groups)
            group->Reset(stop);
    }
}
