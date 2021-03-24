#include <Genode/Audio/Mixer.hpp>

namespace Gx
{
    Mixer::Mixer() :
        m_sources()
    {
    }

    Mixer::~Mixer()
    {
        for (auto [_, sources] : m_sources)
        {
            for (auto source : sources)
                source->stop();

            sources.clear();
        }
    }

    sf::SoundSource* Mixer::Register(sf::SoundSource *source, const std::string &group)
    {
        auto it = std::find_if(m_sources[group].begin(), m_sources[group].end(), [source] (auto sound) { return source == sound.get(); });
        if (it != m_sources[group].end())
            source = it->get();
        else
            m_sources[group].push_back(std::shared_ptr<sf::SoundSource>(source));

        return source;
    }

    void Mixer::Play(sf::SoundSource *source, const std::string &group)
    {
        if (source)
        {
            source = Register(source, group);
            source->play();
        }
    }

    void Mixer::Stop(const std::string &group)
    {
        for (auto source : m_sources[group])
            source->stop();
    }

    void Mixer::StopAll()
    {
        for (auto [_, sources] : m_sources)
        {
            for (auto source : sources)
                source->stop();
        }
    }
}