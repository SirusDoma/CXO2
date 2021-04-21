#include <Genode/Audio/SoundGroup.hpp>

namespace Gx
{
    SoundGroup::SoundGroup(const std::string &name) :
        m_sources(),
        m_volume(100.f),
        m_pan(0.f)
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

    void SoundGroup::Resume()
    {
        for (auto source : m_sources)
        {
            if (source->getStatus() == sf::SoundSource::Paused)
                source->play();
        }
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

    float SoundGroup::GetVolume() const
    {
        return m_volume;
    }

    void SoundGroup::SetVolume(float volume)
    {
        volume = std::min(volume, 100.f);
        volume = std::max(volume, 0.f);

        if (m_volume != volume)
        {
            m_volume = volume;
            for (auto source : m_sources)
                source->setVolume(volume);
        }
    }

    float SoundGroup::GetPan() const
    {
        return m_pan;
    }

    void SoundGroup::SetPan(float pan)
    {
        if (m_pan != pan)
        {
            m_pan = pan;
            for (auto source : m_sources)
                source->setPosition(m_pan, 0.f, 0.f);
        }
    }

    sf::SoundSource* SoundGroup::Play(sf::SoundSource *source)
    {
        if (source)
        {
            auto iterator = std::find(m_sources.begin(), m_sources.end(), source);
            if (iterator == m_sources.end())
                m_sources.push_back(source);

            source->setVolume(m_volume);
            source->setPosition(m_pan, 0.f, 0.f);
            source->play();

            return source;
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
}
