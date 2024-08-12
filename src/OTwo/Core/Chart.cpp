#include <OTwo/Core/Chart.hpp>

#include <vector>

const ChartMetadata &Chart::GetMetadata() const
{
    return m_metadata;
}

void Chart::SetMetadata(const ChartMetadata &metadata)
{
    m_metadata = metadata;
}

std::vector<Chart::Event*> Chart::GetEvents(const Difficulty diff) const
{
    auto events = std::vector<Event*>();
    const auto source = m_events.find(diff);
    if (source == m_events.end())
        return events;
 
    for (const auto &it : source->second)
        events.push_back(it.get());

    return events;
}

void Chart::SortEvents()
{
    for (auto diff : { Difficulty::EX, Difficulty::NX, Difficulty::HX, Difficulty::MX })
    {
        if (m_events.find(diff) == m_events.end())
            continue;

        std::sort(m_events[diff].begin(), m_events[diff].end(), [] (const std::unique_ptr<Event> &a, const std::unique_ptr<Event> &b)
        {
            return a->Position < b->Position;
        });

        // Combine Hold and Release NoteType into one
        // This will preserve the `Hold` note and remove the `Release` note.
        auto holds = std::unordered_map<Channel, NoteEvent*>();
        auto it = m_events[diff].begin();
        while (it != m_events[diff].end())
        {
            const auto ev = it->get();
            if (ev->Channel == Channel::Measurement || ev->Channel == Channel::BPM)
            {
                ++it;
                continue;
            }

            // Long Note transform rule:
            // - Any abnormal playable notes occurence in the middle of holds are treated as `Release` type substitute.
            // - Any abnormal `Release` notes occurence without holds are treated as `Normal` type.
            // - Any hold non-playable events are treated as `Normal` type.
            // - Under any circumstances, do not remove `Normal` non-playable events.
            // - Under any circumstances, remove `Release` type.

            const auto note = static_cast<NoteEvent*>(ev);
            if (holds[ev->Channel])
            {
                holds[ev->Channel]->Length = note->Position - holds[ev->Channel]->Position;
                holds[ev->Channel] = nullptr;

                it = m_events[diff].erase(it);
            }
            else
            {
                if (note->Type == NoteType::Hold)
                {
                    if (ev->IsPlayable())
                        holds[note->Channel] = note;
                    else
                        note->Type = NoteType::Tap;
                }
                else if (note->Type == NoteType::Release)
                {
                    if (!ev->IsPlayable())
                    {
                        it = m_events[diff].erase(it);
                        continue;
                    }
                    else
                        note->Type = NoteType::Tap;
                }

                ++it;
            }
        }
    }
}

void Chart::AddSample(const Gx::Uint16 id, Gx::ResourcePtr<sf::SoundBuffer> sample)
{
    m_samples[id] = std::move(sample);
}

sf::SoundBuffer *Chart::GetSample(const Gx::Uint16 id) const
{
    if (const auto it = m_samples.find(id); it != m_samples.end())
        return it->second.get();

    return nullptr;
}

unsigned int Chart::GetSampleCount() const
{
    return m_samples.size();
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

float Chart::GetMeasureFraction(const int measure) const
{
    if (const auto it = m_fractions.find(measure); it != m_fractions.end())
        return it->second;

    return 1.f;
}

void Chart::SetMeasureFraction(const int measure, const float size)
{
    m_fractions[measure] = size;
}

float Chart::PositionToSeconds(const float position, const float bpm)
{
    return position * 4 * (60 / bpm);
}

float Chart::SecondsToPosition(const float seconds, const float bpm)
{
    return seconds / (4 * (60 / bpm));
}

