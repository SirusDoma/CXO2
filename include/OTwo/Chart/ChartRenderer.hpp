#ifndef O2JAM_CHART_RENDERER_HPP
#define O2JAM_CHART_RENDERER_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/Chart/Chart.hpp>
#include <OTwo/Contexts/GameContext.hpp>

#include <Genode/Graphics/Animation.hpp>

#include <unordered_map>

using NotePrefabMap = std::unordered_map<NoteShape, Gx::Sprite*>;

using ChannelSet = std::unordered_set<Chart::Channel>;
using SpeedMap = std::unordered_map<Chart::Channel, float>;

class NoteContainer;
class ChartRenderer : public Gx::Renderable
{
public:
    struct RenderSettings
    {
        GameConfig   *Config;
        unsigned int  Viewport;
        float         Speed;
        Difficulty    Difficulty;

        // TODO: Modifiers
    };

    static constexpr unsigned int DefaultMeasureHeight = 384;

    explicit ChartRenderer(State &state, const ChannelSet &instantiables = {});

    void Render(const Chart &chart, const GameContext &context);
    void Render(const Chart &chart, const RenderSettings &settings);

    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;
    void Input(Chart::Channel channel, bool pressed);

    const RenderSettings &GetRenderSettings() const;
    float GetSpeed(Chart::Channel channel) const;
    double GetCurrentTime() const;
    double GetRenderPosition() const;
    double GetCurrentBPM() const;

    int MapRenderPositionToPixels(Chart::Channel channel, double position, bool absolute = false) const;

private:
    // Measure interval per millisecond @ 60bpm in 1/4 note
    static constexpr double TickSignature = 60000.f * 4.f;

    struct EventState
    {
        Chart::Event *Event;
        Accuracy      Accuracy = Accuracy::None;
        double        Latency  = 0;

        Chart::Event *operator->() const { return Event; }
    };

    using PrefabMap      = std::unordered_map<Chart::Channel, std::unordered_map<Chart::NoteType, NotePrefabMap>>;
    using EventStateList = std::vector<EventState>;
    using FrontBufferMap = std::unordered_map<Chart::Channel, EventState*>;
    using InputStateMap  = std::unordered_map<Chart::Channel, bool>;
    using SoundMap       = std::unordered_map<unsigned int, sf::Sound*>;

    State *m_parent;
    NoteContainer* m_container;

    const Chart *m_chart;
    RenderSettings m_settings;

    ChannelSet m_instantiables;
    SpeedMap m_speeds;
    InputStateMap m_inputs;
    SoundMap m_sounds;

    sf::Clock m_timer;
    mutable PrefabMap m_prefabs;
    mutable EventStateList m_events;
    mutable FrontBufferMap m_frontBuffers;
    mutable double m_currentTime;
    mutable double m_refTime;
    mutable double m_refPosition;
    mutable double m_bpm;
    mutable unsigned int m_frameId;
};

#endif
