#ifndef O2JAM_CORE_CHART_HPP
#define O2JAM_CORE_CHART_HPP

#include <OTwo/Metadata/Chart/ChartMetadata.hpp>
#include <OTwo/Models/Game.hpp>

#include <Genode/System/Primitives.hpp>

#include <SFML/Audio/SoundBuffer.hpp>

#include <vector>
#include <memory>
#include <map>
#include <unordered_map>

class Chart
{
public:
    enum class Channel : Gx::Uint16
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
        Background
    };

    enum class NoteType : Gx::Uint8
    {
        Tap     = 0,
        Hold    = 2,
        Release = 3,
        Sample  = 4
    };

    static constexpr std::array<Channel, 7> PlayableChannels = {
        Channel::Note1,
        Channel::Note2,
        Channel::Note3,
        Channel::Note4,
        Channel::Note5,
        Channel::Note6,
        Channel::Note7
    };

    static constexpr std::array<Channel, 8> NoteChannels = {
        Channel::Note1,
        Channel::Note2,
        Channel::Note3,
        Channel::Note4,
        Channel::Note5,
        Channel::Note6,
        Channel::Note7,
        Channel::Background
    };

    struct Event
    {
        Chart::Channel Channel;
        double         Position;

        Event(const Chart::Channel channel, const double position) :
            Channel(channel),
            Position(position)
        {
        }

        virtual ~Event() = default;

        bool IsPlayable() const
        {
            return Channel != Channel::Measurement &&
                   Channel != Channel::BPM &&
                   Channel != Channel::Background;
        }
    };

    struct TimeEvent : Event
    {
        float Value;

        TimeEvent(const Event &ev, const float value) :
           Event(ev),
           Value(value)
        {
        }
    };

    struct NoteEvent : Event
    {
        Gx::Uint16       ID;
        float            Volume;
        float            Pan;
        NoteType         Type;
        sf::SoundBuffer *Sample;
        double           Length;

        NoteEvent(const Event &ev, const Gx::Uint16 id, const float volume, const float pan, const NoteType type, sf::SoundBuffer *sample) :
            Event(ev),
            ID(id),
            Volume(volume),
            Pan(pan),
            Type(type),
            Sample(sample),
            Length(0)
        {
        }
    };

    using EventList   = std::vector<std::unique_ptr<Event>>;

    Chart() = default;

    const ChartMetadata &GetMetadata() const;
    void SetMetadata(const ChartMetadata &metadata);

    template<typename T>
    T* AddEvent(Difficulty diff, T &&ev);
    const EventList& GetEvents(Difficulty diff) const;
    void SortEvents();

    void AddSample(Gx::Uint16 id, Gx::ResourcePtr<sf::SoundBuffer> sample);
    sf::SoundBuffer *GetSample(Gx::Uint16 id) const;
    unsigned int GetSampleCount() const;

    const sf::Image *GetCover() const;
    void SetCover(Gx::ResourcePtr<sf::Image> cover);

    const sf::Image *GetThumbnail() const;
    void SetThumbnail(Gx::ResourcePtr<sf::Image> thumbnail);

    std::map<int, float> GetMeasureFractions(Difficulty diff) const;
    float GetMeasureFraction(Difficulty diff, int measure) const;
    void SetMeasureFraction(Difficulty diff, int measure, float size);

    double GetLastEventPosition(Difficulty diff) const;

    static float PositionToSeconds(float position, float bpm);
    static float SecondsToPosition(float seconds, float bpm);

    std::string Source;
private:
    using EventMap    = std::unordered_map<Difficulty, EventList>;
    using SampleMap   = std::unordered_map<Gx::Uint16, Gx::ResourcePtr<sf::SoundBuffer>>;
    using FractionMap = std::unordered_map<Difficulty, std::map<int, float>>;
    using PositionMap = std::unordered_map<Difficulty, double>;

    ChartMetadata m_metadata;
    EventMap      m_events;
    SampleMap     m_samples;
    FractionMap   m_fractions;
    PositionMap   m_lasts;

    Gx::ResourcePtr<sf::Image> m_cover;
    Gx::ResourcePtr<sf::Image> m_thumbnail;
};

#include <OTwo/Core/Chart.inl>
#endif
