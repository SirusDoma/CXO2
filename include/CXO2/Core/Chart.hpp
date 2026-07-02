#pragma once

#include <CXO2/Metadata/Chart/O2JamChartMetadata.hpp>
#include <CXO2/Models/Game.hpp>

#include <Genode/IO/Resource.hpp>

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Image.hpp>

#include <vector>
#include <memory>
#include <map>
#include <set>
#include <unordered_map>

namespace Cx
{
    class Chart
    {
    public:
        enum class Channel : std::uint16_t
        {
            Measure     = 0,
            BPM         = 1,
            Note1       = 2,
            Note2       = 3,
            Note3       = 4,
            Note4       = 5,
            Note5       = 6,
            Note6       = 7,
            Note7       = 8,

            // WARNING: Comparison against Chart::Channel background should be avoided
            Background
        };

        enum class NoteType : std::uint8_t
        {
            Tap     = 0,
            Hold    = 2,
            Release = 3
        };

        enum class SampleType : std::uint8_t
        {
            KeySound   = 0,
            Background = 4
        };

        inline static const std::set<Channel> PlayableChannels = {
            Channel::Note1,
            Channel::Note2,
            Channel::Note3,
            Channel::Note4,
            Channel::Note5,
            Channel::Note6,
            Channel::Note7
        };

        inline static const std::set<Channel> NonPlayableChannels = {
            Channel::Measure,
            Channel::BPM
        };

        inline static const std::set<Channel> TimeChannels = NonPlayableChannels;

        inline static const std::set<Channel> NoteChannels = {
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
            Cx::Chart::Channel Channel;
            double  Position;

            Event(const Chart::Channel channel, const double position) :
                Channel(channel),
                Position(position)
            {
            }

            virtual ~Event() = default;

            bool IsPlayable() const
            {
                return PlayableChannels.find(Channel) != PlayableChannels.end();
            }

            virtual bool operator==(const Event &other) const
            {
                return Channel == other.Channel && Position == other.Position;
            }
        };

        struct TimeEvent : Event
        {
            float Value;

            TimeEvent(const Event& ev, const float value) :
               Event(ev),
               Value(value)
            {
            }

            bool operator==(const Event &other) const override
            {
                const auto time = dynamic_cast<const TimeEvent*>(&other);
                return time && Event::operator==(other) && Value == time->Value;
            }
        };

        struct NoteEvent : Event
        {
            std::uint16_t         ID;
            float                 Volume;
            float                 Pan;
            NoteType              Type;
            Cx::Chart::SampleType SampleType;
            sf::SoundBuffer*      Sample;
            double                Length;

            NoteEvent(const Event& ev, const std::uint16_t id, const float volume, const float pan, const NoteType type, const enum SampleType sampleType, sf::SoundBuffer* sample) :
                Event(ev),
                ID(id),
                Volume(volume),
                Pan(pan),
                Type(type),
                SampleType(sampleType),
                Sample(sample),
                Length(0)
            {
            }

            bool operator==(const Event &other) const override
            {
                const auto note = dynamic_cast<const NoteEvent*>(&other);
                return note && Event::operator==(other) && ID == note->ID;
            }
        };

        using EventList   = std::vector<std::unique_ptr<Event>>;

        Chart() = default;

        const ChartMetadata& GetMetadata() const;
        void SetMetadata(const ChartMetadata& metadata);

        template<typename T>
        T* AddEvent(Difficulty diff, T &&ev);
        const EventList& GetEvents(Difficulty diff) const;
        std::size_t GetEventCount(Difficulty diff) const;
        void SortEvents();

        void AddSample(std::uint16_t id, Gx::ResourcePtr<sf::SoundBuffer> sample);
        sf::SoundBuffer* GetSample(std::uint16_t id) const;
        unsigned int GetSampleCount() const;

        const sf::Image* GetCover() const;
        void SetCover(Gx::ResourcePtr<sf::Image> cover);

        const sf::Image* GetThumbnail() const;
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
        using SampleMap   = std::unordered_map<std::uint16_t, Gx::ResourcePtr<sf::SoundBuffer>>;
        using FractionMap = std::unordered_map<Difficulty, std::map<int, float>>;
        using PositionMap = std::unordered_map<Difficulty, double>;

        ChartMetadata  m_metadata;
        EventMap           m_events;
        SampleMap          m_samples;
        FractionMap        m_fractions;
        PositionMap        m_lasts;

        Gx::ResourcePtr<sf::Image> m_cover;
        Gx::ResourcePtr<sf::Image> m_thumbnail;
    };
}

template <>
struct std::hash<Cx::Chart::Event>
{
    std::size_t operator()(const Cx::Chart::Event& key) const noexcept
    {
        std::size_t seed = 0;
        HashMerge(seed, static_cast<int>(key.Channel));
        HashMerge(seed, key.Position);
        return seed;
    }

private:
    template <class T>
    void HashMerge(std::size_t& seed, const T& val) const
    {
        seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
};

template <>
struct std::hash<Cx::Chart::NoteEvent>
{
    std::size_t operator()(const Cx::Chart::NoteEvent& key) const noexcept
    {
        std::size_t seed = 0;
        HashMerge(seed, static_cast<int>(key.Channel));
        HashMerge(seed, key.Position);
        HashMerge(seed, key.ID);
        return seed;
    }

private:
    template <class T>
    void HashMerge(std::size_t& seed, const T& val) const
    {
        seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
};

#include <CXO2/Core/Chart.inl>
