#include <Genode/Audio/SoundGroup.hpp>

namespace Gx
{
    SoundGroup::SoundGroup() :
        SoundGroup::SoundGroup("default")
    {
    }

    SoundGroup::SoundGroup(const std::string &name) :
        m_sources()
    {
        SetName(name);
    }

    SoundGroup::~SoundGroup()
    {
        m_sources.clear();
    }

    const std::string &SoundGroup::GetName() const
    {
        return m_name;
    }

    void SoundGroup::SetName(const std::string &name)
    {
        m_name = name;
    }

    sf::SoundSource* SoundGroup::Play(sf::SoundSource *source)
    {
        if (source)
        {
            auto iterator = std::find(m_sources.begin(), m_sources.end(), source);
            if (iterator == m_sources.end())
            {
                m_sources.push_back(source);
                source->play();

                return source;
            }
            else
            {
                source->play();
                return *iterator;
            }
        }

        return nullptr;
    }

    bool SoundGroup::Remove(sf::SoundSource *source)
    {
        if (source)
        {
            auto iterator = std::find(m_sources.begin(), m_sources.end(), source);
            if (iterator != m_sources.end())
                return m_sources.erase(iterator) == m_sources.end();
        }

        return false;
    }

    void SoundGroup::Play()
    {
        for (auto source : m_sources)
            source->play();
    }

    void SoundGroup::Pause()
    {
        for (auto source : m_sources)
            source->pause();
    }

    void SoundGroup::Stop()
    {
        for (auto source : m_sources)
            source->stop();
    }

    void SoundGroup::SetVolume(float volume)
    {
        for (auto source : m_sources)
            source->setVolume(volume);
    }

    void SoundGroup::SetPan(float pan)
    {
        for (auto source : m_sources)
            source->setPosition(pan, 0.f, 0.f);
    }
}
