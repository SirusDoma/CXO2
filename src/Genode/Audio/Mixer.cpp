#include <Genode/Audio/Mixer.hpp>
#include <Genode/Audio/SoundGroup.hpp>

namespace Gx
{
    Mixer::Mixer() :
        m_groups()
    {
    }

    Mixer::~Mixer()
    {
        m_groups.clear();
    }

    sf::SoundSource* Mixer::Play(sf::SoundSource *source, const std::string &groupName)
    {
        if (source)
        {
            auto iterator = m_groups.find(groupName);
            if (iterator == m_groups.end())
            {
                m_groups[groupName] = std::make_unique<SoundGroup>(groupName);
                return m_groups[groupName]->Play(source);
            }
            else
                return iterator->second->Play(source);
        }

        return nullptr;
    }

    void Mixer::Pause(const std::string &group)
    {
        if (m_groups.find(group) != m_groups.end())
            m_groups[group]->Pause();
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

    void Mixer::StopAll()
    {
        for (auto& [_, group] : m_groups)
            group->Stop();

        m_groups.clear();
    }

    SoundGroup *Mixer::GetGroup(const std::string &group)
    {
        auto iterator = m_groups.find(group);
        if (iterator != m_groups.end())
            return iterator->second.get();

        return nullptr;
    }

    void Mixer::Update(double delta)
    {
    }
}
