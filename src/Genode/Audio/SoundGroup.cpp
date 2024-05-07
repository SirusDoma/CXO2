#include <Genode/Audio/SoundGroup.hpp>

#include <cmath>

namespace Gx
{
    SoundGroup::SoundGroup(const std::string &name) :
        m_sources(),
        m_volume(100.f),
        m_pan(0.f),
        m_enabled(true)
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

    sf::SoundSource::Status SoundGroup::GetStatus() const
    {
        bool paused = false;
        for (const auto source : m_sources)
        {
            if (source->getStatus() == sf::SoundSource::Playing)
                return sf::SoundSource::Playing;
            else if (source->getStatus() == sf::SoundSource::Paused)
                paused = true;
        }

        if (paused)
            return sf::SoundSource::Paused;

        return sf::SoundSource::Stopped;
    }

    void SoundGroup::Play()
    {
        if (!m_enabled)
            return;

        for (const auto source : m_sources)
        {
            // TODO: expose playing offset?
            if (const auto sound = dynamic_cast<sf::Sound*>(source); sound)
                sound->setPlayingOffset(sf::Time::Zero);
            else if (const auto music = dynamic_cast<sf::Music*>(source); music)
                music->setPlayingOffset(sf::Time::Zero);
            else
                source->stop(); // isn't thread-safe

            source->play();
        }
    }

    void SoundGroup::Resume()
    {
        if (!m_enabled)
            return;

        for (const auto source : m_sources)
        {
            if (source->getStatus() == sf::SoundSource::Paused)
                source->play();
        }
    }

    void SoundGroup::Pause()
    {
        if (!m_enabled)
            return;

        for (const auto source : m_sources)
            source->pause();
    }

    void SoundGroup::Stop()
    {
        if (!m_enabled)
            return;

        for (const auto source : m_sources)
            source->stop();

        m_sources.clear();
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

    sf::SoundSource* SoundGroup::Play(sf::SoundSource *source)
    {
        if (source && m_enabled)
        {
            const auto iterator = std::find(m_sources.begin(), m_sources.end(), source);
            if (iterator == m_sources.end())
                m_sources.push_back(source);

            source->stop();
            source->setVolume(m_volume);
            source->setPosition(sf::Vector3f(m_pan, 0.f, 0.f));
            source->play();

            return source;
        }

        return nullptr;
    }

    bool SoundGroup::Remove(sf::SoundSource *source)
    {
        if (source && m_enabled)
        {
            const auto iterator = std::find(m_sources.begin(), m_sources.end(), source);
            if (iterator != m_sources.end())
                return m_sources.erase(iterator) == m_sources.end();
        }

        return false;
    }

    void SoundGroup::Update(const double delta)
    {
        m_sources.erase(std::remove_if(m_sources.begin(), m_sources.end(), [] (const sf::SoundSource *src)
        {
            return !src || src->getStatus() == sf::SoundSource::Stopped;
        }), m_sources.end());
    }

    void SoundGroup::Clear()
    {
        m_sources.clear();
    }
}
