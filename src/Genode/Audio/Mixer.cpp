#include <Genode/Audio/Mixer.hpp>
#include <Genode/Audio/SoundGroup.hpp>

namespace Gx
{
    Mixer::Mixer() :
        m_masterGroup(),
        m_groups(),
        m_sources(),
        m_resources()
    {
        m_masterGroup = std::unique_ptr<SoundGroup>(new SoundGroup("master"));
    }

    Mixer::Mixer(ResourceManager &sharedResource) :
        m_masterGroup(),
        m_groups(),
        m_sources(),
        m_resources(&sharedResource)
    {
        m_masterGroup = std::unique_ptr<SoundGroup>(new SoundGroup("master"));
    }

    Mixer::Mixer(Mixer &&other) :
        m_masterGroup(),
        m_groups(),
        m_sources(),
        m_resources()
    {
        *this = std::move(other);
    }

    Mixer::~Mixer()
    {
        m_groups.clear();
        m_sources.clear();
    }

    SoundGroup *Mixer::GetMasterSoundGroup() const
    {
        return m_masterGroup.get();
    }

    SoundGroup *Mixer::GetSoundGroup(const std::string &group) const
    {
        auto iterator = m_groups.find(group);
        if (iterator != m_groups.end())
            return iterator->second.get();

        return nullptr;
    }

    sf::SoundSource* Mixer::Play(sf::SoundSource *source)
    {
        return Play(source, m_masterGroup.get());
    }

    sf::SoundSource *Mixer::Play(sf::SoundSource *source, std::string groupName)
    {
        if (source)
        {
            if (groupName.empty() || groupName == "master")
                return Play(source, m_masterGroup.get());

            if (auto iterator = m_groups.find(groupName); iterator == m_groups.end())
                m_groups[groupName] = std::unique_ptr<SoundGroup>(new SoundGroup(groupName));

            return Play(source, m_groups[groupName].get());
        }

        return nullptr;
    }

    sf::SoundSource *Mixer::Play(sf::SoundSource *source, SoundGroup *group)
    {
        if (source)
        {
            if (!group)
                return Play(source);

            if (group != m_masterGroup.get())
                m_masterGroup->Remove(source);

            for (auto& iterator : m_groups)
            {
                if (iterator.second.get() != group)
                    iterator.second->Remove(source);
            }

            return group->Play(source);
        }

        return nullptr;
    }

    void Mixer::Pause(const std::string &group)
    {
        if (m_groups.find(group) != m_groups.end())
            Pause(m_groups[group].get());
    }

    void Mixer::Pause(SoundGroup *group)
    {
        if (group)
            group->Pause();
    }

    void Mixer::Resume(const std::string &group)
    {
        if (m_groups.find(group) != m_groups.end())
            Resume(m_groups[group].get());
    }

    void Mixer::Resume(SoundGroup *group)
    {
        if (group)
            group->Resume();
    }

    void Mixer::Stop(const std::string &group)
    {
        auto iterator = m_groups.find(group);
        if (iterator != m_groups.end())
        {
            m_groups[group]->Stop();
            m_groups.erase(iterator);
        }
    }

    void Mixer::Stop(SoundGroup *group)
    {
        if (group)
        {
            group->Stop();
            if (auto iterator = m_groups.find(group->GetName()); iterator != m_groups.end())
                m_groups.erase(iterator);
        }
    }

    void Mixer::SetVolume(float volume)
    {
        m_masterGroup->SetVolume(volume);
        for (auto& [_, group] : m_groups)
            group->SetVolume(volume);
    }

    void Mixer::SetPan(float pan)
    {
        m_masterGroup->SetPan(pan);
        for (auto& [_, group] : m_groups)
            group->SetPan(pan);
    }

    void Mixer::ResumeAll()
    {
        m_masterGroup->Resume();
        for (auto& [_, group] : m_groups)
            group->Resume();
    }

    void Mixer::PauseAll()
    {
        m_masterGroup->Pause();
        for (auto& [_, group] : m_groups)
            group->Pause();
    }

    void Mixer::StopAll()
    {
        m_masterGroup->Stop();
        for (auto& [_, group] : m_groups)
            group->Stop();

        m_groups.clear();
    }

    void Mixer::Update(double delta)
    {
    }

    Mixer &Mixer::operator=(Mixer &&right)
    {
        m_masterGroup = std::move(right.m_masterGroup);
        m_groups      = std::move(right.m_groups);
        m_sources     = std::move(right.m_sources);
        m_resources   = std::move(right.m_resources);

        return *this;
    }
}
