#ifndef O2JAM_CHART_DATA_HPP
#define O2JAM_CHART_DATA_HPP

#include <OTwo/Metadata/Chart/O2ChartMetadata.hpp>

#include <unordered_map>
#include <vector>

class O2Chart
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
        Gx::Uint32 Measure;
        Gx::Uint32 Beat;
        Gx::Uint32 Cell;
        Gx::Uint16 Signature;
    };

    struct TimeEvent : public Event
    {
        using Event::Event;
        float Value;
    };

    struct NoteEvent : public Event
    {
        using Event::Event;
        Gx::Uint32 ID;
        Gx::Uint8  VolumePan;
        NoteType   Type;
    };

    O2Chart() = default;

    const O2ChartMetadata &GetMetadata() const;
    void SetMetadata(O2ChartMetadata &&metadata);

    void AddEvent(Difficulty diff, std::unique_ptr<Event> ev);
    const std::vector<Event*> GetEvents(Difficulty diff) const;

private:
    using EventMap = std::unordered_map<Difficulty, std::vector<std::unique_ptr<Event>>>;

    O2ChartMetadata m_metadata;
    EventMap        m_events;
};

#endif
