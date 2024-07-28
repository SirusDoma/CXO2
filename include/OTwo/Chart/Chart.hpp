#ifndef O2JAM_CHART_DATA_HPP
#define O2JAM_CHART_DATA_HPP

#include <OTwo/Metadata/Chart/ChartMetadata.hpp>
#include <OTwo/Models/Game.hpp>

#include <Genode/System/Primitives.hpp>

#include <SFML/Audio/SoundBuffer.hpp>

#include <map>
#include <vector>
#include <memory>

class Chart
{
public:
    enum class ChannelType : Gx::Uint16
    {
        Measurement = 0,
        BPM         = 1,
        Note1       = 2,
        Note2       = 3,
        Note3       = 4,
        Note4       = 5,
        Note5       = 6,
        Note6       = 7,
        Note7       = 8,
        BGM
    };

    enum class NoteType : Gx::Uint8
    {
        Normal  = 0,
        Hold    = 2,
        Release = 3,
        Sample  = 4
    };

    struct Event
    {
        float       Position;
        ChannelType Channel;

        bool IsPlayable() const
        {
            return Channel != ChannelType::Measurement &&
                   Channel != ChannelType::BPM &&
                   Channel != ChannelType::BGM;
        }
    };

    struct TimeEvent : public Event
    {
        float Value;
    };

    struct NoteEvent : public Event
    {
        Gx::Uint16       ID;
        float            Volume;
        float            Pan;
        NoteType         Type;
        sf::SoundBuffer *Sample;
    };

    Chart() = default;

    const ChartMetadata &GetMetadata() const;
    void SetMetadata(const ChartMetadata &metadata);

    template<typename T>
    void AddEvent(Difficulty diff, T &&ev);
    std::vector<Event*> GetEvents(Difficulty diff) const;

    void AddSample(Gx::Uint16 id, Gx::ResourcePtr<sf::SoundBuffer> sample);
    sf::SoundBuffer *GetSample(Gx::Uint16 id) const;

    const sf::Image *GetCover() const;
    void SetCover(Gx::ResourcePtr<sf::Image> cover);

    const sf::Image *GetThumbnail() const;
    void SetThumbnail(Gx::ResourcePtr<sf::Image> thumbnail);

    static float PositionToSeconds(float position, float bpm);
    static float SecondsToPosition(float seconds, float bpm);

    std::string Source;
private:
    using EventList = std::vector<std::unique_ptr<Event>>;
    using EventMap  = std::unordered_map<Difficulty, EventList>;
    using SampleMap = std::unordered_map<Gx::Uint16, Gx::ResourcePtr<sf::SoundBuffer>>;

    ChartMetadata m_metadata;
    EventMap      m_events;
    SampleMap     m_samples;

    Gx::ResourcePtr<sf::Image> m_cover;
    Gx::ResourcePtr<sf::Image> m_thumbnail;
};

#include <OTwo/Chart/Chart.inl>
#endif
