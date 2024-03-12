#include <Genode/Audio/Mixer.hpp>
#include <Genode/Audio/SoundGroup.hpp>

namespace Gx
{
    Mixer::Mixer() :
        m_masterGroup(),
        m_groups(),
        m_sources()
    {
        m_masterGroup = ResourcePtr<SoundGroup>(new SoundGroup("master"), [] (auto ptr)
        {
            ptr->Stop();
            delete ptr;
        });
    }

    Mixer::Mixer(Mixer &&right) noexcept:
        m_masterGroup(std::move(right.m_masterGroup)),
        m_groups(std::move(right.m_groups)),
        m_sources(std::move(right.m_sources))
    {
    }

    Mixer &Mixer::operator=(Mixer &&right) noexcept
    {
        m_masterGroup = std::move(right.m_masterGroup);
        m_groups      = std::move(right.m_groups);
        m_sources     = std::move(right.m_sources);

        return *this;
    }

    Mixer::~Mixer()
    {
    }

    SoundGroup *Mixer::GetMasterSoundGroup() const
    {
        return m_masterGroup.get();
    }

    SoundGroup *Mixer::GetSoundGroup(const std::string &group)
    {
        auto iterator = m_groups.find(group);
        if (iterator == m_groups.end())
            m_groups[group] = ResourcePtr<SoundGroup>(new SoundGroup(group), [] (auto ptr) { delete ptr; });

        return m_groups[group].get();
    }

    sf::SoundSource* Mixer::Play(sf::SoundSource *source)
    {
        return Play(source, m_masterGroup.get());
    }

    sf::SoundSource *Mixer::Play(sf::SoundSource *source, const std::string &group)
    {
        if (source)
        {
            if (group.empty() || group == "master")
                return Play(source, m_masterGroup.get());

            if (auto iterator = m_groups.find(group); iterator == m_groups.end())
                m_groups[group] = ResourcePtr<SoundGroup>(new SoundGroup(group), [] (auto ptr) { delete ptr; });

            return Play(source, m_groups[group].get());
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

    void Mixer::Play(const std::string &groupName)
    {
        if (!groupName.empty())
        {
            SoundGroup *group = nullptr;
            if (groupName != "master")
            {
                if (auto iterator = m_groups.find(groupName); iterator != m_groups.end())
                    group = iterator->second.get();
            }
            else
                group = m_masterGroup.get();

            if (!group)
                return;

            group->Play();
        }
    }

    void Mixer::Play(SoundGroup *group)
    {
        if (group)
            group->Play();
    }

    void Mixer::Pause(const std::string &group)
    {
        if (m_groups.find(group) != m_groups.end())
            Pause(m_groups[group].get());
    }

    void Mixer::Pause(sf::SoundSource *source)
    {
        if (source)
            source->pause();
    }

    void Mixer::Pause(SoundGroup *group)
    {
        if (group)
            group->Pause();
    }

    void Mixer::Resume(sf::SoundSource *source)
    {
        if (source && source->getStatus() == sf::SoundSource::Paused)
            source->play();
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

    void Mixer::Stop(sf::SoundSource *source)
    {
        if (source)
        {
            source->stop();
            Update(0);
        }
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

    void Mixer::PlayAll()
    {
        m_masterGroup->Play();
        for (auto& [_, group] : m_groups)
            group->Play();
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
        {
            group->Stop();
            group->Clear();
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

    void Mixer::Update(double delta)
    {
        for (auto& [_, group] : m_groups)
            group->Update(delta);
    }

    void Mixer::Clear()
    {
        m_sources.clear();
        m_groups.clear();
    }
}
