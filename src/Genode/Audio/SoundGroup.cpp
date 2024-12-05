#include <Genode/Audio/SoundGroup.hpp>

#include <algorithm>
#include <cmath>

namespace Gx
{
    SoundGroup::SoundGroup(const std::string& name) :
        m_volume(100.f),
        m_pan(0.f),
        m_enabled(true),
        m_sources()
    {
        SetName(name);
    }

    const std::string& SoundGroup::GetName() const
    {
        return m_name;
    }

    void SoundGroup::SetName(const std::string& name)
    {
        m_name = name;
    }

    void SoundGroup::Play() const
    {
        if (!m_enabled)
            return;

        for (const auto source : m_sources)
        {
            if (source->getStatus() == sf::SoundSource::Status::Paused)
            {
                if (const auto sound = dynamic_cast<sf::Sound*>(source))
                    sound->setPlayingOffset(sf::Time::Zero);
                else if (const auto music = dynamic_cast<sf::Music*>(source))
                    music->setPlayingOffset(sf::Time::Zero);
                else if (const auto stream = dynamic_cast<sf::SoundStream*>(source))
                    stream->setPlayingOffset(sf::Time::Zero);
            }

            source->play();
        }
    }

    void SoundGroup::Resume() const
    {
        if (!m_enabled)
            return;

        for (const auto source : m_sources)
        {
            if (source->getStatus() == sf::SoundSource::Status::Paused)
                source->play();
        }
    }

    void SoundGroup::Pause() const
    {
        if (!m_enabled)
            return;

        for (const auto& source : m_sources)
            source->pause();
    }

    void SoundGroup::Stop() const
    {
        if (!m_enabled)
            return;

        for (const auto source : m_sources)
            source->stop();
    }

    void SoundGroup::SetPlayingOffset(const sf::Time timeOffset) const
    {
        if (!m_enabled)
            return;

        for (const auto source : m_sources)
        {
            if (const auto sound = dynamic_cast<sf::Sound*>(source))
                sound->setPlayingOffset(timeOffset);
            else if (const auto music = dynamic_cast<sf::Music*>(source))
                music->setPlayingOffset(timeOffset);
            else if (const auto stream = dynamic_cast<sf::SoundStream*>(source))
                stream->setPlayingOffset(timeOffset);
        }
    }

    float SoundGroup::GetVolume() const
    {
        return m_volume;
    }

    void SoundGroup::SetVolume(float volume)
    {
        if (!m_enabled)
            return;

        volume = std::min(volume, 100.f);
        volume = std::max(volume, 0.f);

        if (m_volume != volume)
        {
            m_volume = volume;
            for (const auto source : m_sources)
                source->setVolume(volume);
        }
    }

    float SoundGroup::GetPan() const
    {
        return m_pan;
    }

    void SoundGroup::SetPan(const float pan)
    {
        if (m_pan != pan && m_enabled)
        {
            m_pan = pan;
            for (const auto source : m_sources)
                source->setPosition(sf::Vector3f(m_pan, 0.f, 0.f));
        }
    }

    bool SoundGroup::IsEnabled() const
    {
        return m_enabled;
    }

    void SoundGroup::SetEnabled(const bool enable)
    {
        if (!enable)
            Pause();

        m_enabled = enable;
    }

    sf::SoundSource& SoundGroup::Play(sf::SoundSource& source)
    {
        if (m_enabled)
        {
            m_sources.insert(&source);

            source.stop();
            source.setVolume(m_volume);
            source.setPosition(sf::Vector3f(m_pan, 0.f, 0.f));

            if (source.getStatus() != sf::SoundSource::Status::Playing)
                source.play();
        }

        return source;
    }

    bool SoundGroup::Remove(const sf::SoundSource& source)
    {
        if (m_enabled)
        {
            const auto iterator = std::find(m_sources.begin(), m_sources.end(), &source);
            if (iterator != m_sources.end())
                return m_sources.erase(iterator) == m_sources.end();
        }

        return false;
    }

    void SoundGroup::Reset(const bool stop)
    {
        if (stop)
            Stop();

        m_sources.clear();
    }
}

namespace Gx
{
    SoundGroup::Iterator::Iterator(SoundGroup& soundGroup, const bool end) :
        m_soundGroup(soundGroup),
        m_iterator(end ? soundGroup.m_sources.end() : soundGroup.m_sources.begin())
    {
    }

    SoundGroup::Iterator::IteratorHandle SoundGroup::Iterator::begin() const
    {
        return m_soundGroup.m_sources.begin();
    }

    SoundGroup::Iterator::IteratorHandle SoundGroup::Iterator::end() const
    {
        return m_soundGroup.m_sources.end();
    }

    bool SoundGroup::Iterator::operator!=(const Iterator& other) const
    {
        return m_iterator != other.m_iterator;
    }

    sf::SoundSource& SoundGroup::Iterator::operator*() const
    {
        return **m_iterator;
    }

    SoundGroup::Iterator& SoundGroup::Iterator::operator++()
    {
        ++m_iterator;
        return *this;
    }

    SoundGroup::Iterator begin(SoundGroup& soundGroup)
    {
        return SoundGroup::Iterator(soundGroup);
    }

    SoundGroup::Iterator end(SoundGroup& soundGroup)
    {
        return SoundGroup::Iterator(soundGroup, true);
    }
}
