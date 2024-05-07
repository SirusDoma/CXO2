#include <OTwo/Chart/Chart.hpp>

#include <vector>

const ChartMetadata &Chart::GetMetadata() const
{
    return m_metadata;
}

void Chart::SetMetadata(const ChartMetadata &metadata)
{
    m_metadata = metadata;
}

std::vector<Chart::Event*> Chart::GetEvents(Difficulty diff) const
{
    auto events = std::vector<Event*>();
    const auto source = m_events.find(diff);
    if (source == m_events.end())
        return events;

    for (const auto &it : source->second)
        events.push_back(it.get());

    return events;
}

void Chart::AddSample(Gx::Uint16 id, Gx::ResourcePtr<sf::SoundBuffer> sample)
{
    m_samples[id] = std::move(sample);
}

sf::SoundBuffer *Chart::GetSample(Gx::Uint16 id) const
{
    if (const auto it = m_samples.find(id); it != m_samples.end())
        return it->second.get();

    return nullptr;
}

const sf::Image *Chart::GetCover() const
{
    return m_cover.get();
}

void Chart::SetCover(Gx::ResourcePtr<sf::Image> cover)
{
    m_cover = std::move(cover);
}

const sf::Image *Chart::GetThumbnail() const
{
    return m_thumbnail.get();
}

void Chart::SetThumbnail(Gx::ResourcePtr<sf::Image> thumbnail)
{
    m_thumbnail = std::move(thumbnail);
}
