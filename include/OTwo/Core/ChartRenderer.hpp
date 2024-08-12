#ifndef O2JAM_CORE_CHART_RENDERER_HPP
#define O2JAM_CORE_CHART_RENDERER_HPP

#include <OTwo/Core/Chart.hpp>
#include <OTwo/Core/JudgementStrategy.hpp>
#include <OTwo/Core/ScoreTracker.hpp>

#include <OTwo/States/State.hpp>
#include <OTwo/Contexts/GameContext.hpp>
#include <OTwo/UI/Playing/ComboCounter.hpp>
#include <OTwo/UI/Playing/JudgementIndicator.hpp>
#include <OTwo/UI/Playing/PlayMenu.hpp>

#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/Number.hpp>

#include <unordered_map>

using NotePrefabMap = std::unordered_map<NoteShape, Gx::Sprite*>;

using ChannelSet = std::unordered_set<Chart::Channel>;
using SpeedMap = std::unordered_map<Chart::Channel, float>;

class NoteContainer;
class JudgementStrategy;
class ChartRenderer : public virtual Gx::Node, public Gx::RenderableContainer, public Gx::UpdatableContainer
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
    void Input(Chart::Channel channel, bool pressed) const ;

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
        Judgement    Tap     = {Accuracy::None, 0.f};
        Judgement    Release = {Accuracy::None, 0};

        bool IsRegistered() const
        {
            const auto note = dynamic_cast<Chart::NoteEvent*>(Event);
            return Tap.Accuracy != Accuracy::None && ((note && note->Length == 0) || Release.Accuracy != Accuracy::None);
        }

        Chart::Event *operator->() const { return Event; }
    };

    void PlaySample(const Chart::NoteEvent* ev, const std::string &group = "BGM") const;
    void OnScoreUpdated(const Chart::NoteEvent& ev, Accuracy acc, unsigned int count) const;

    using PrefabMap      = std::unordered_map<Chart::Channel, std::unordered_map<Chart::NoteType, NotePrefabMap>>;
    using AnimationMap   = std::unordered_map<Chart::Channel, Gx::Animation*>;
    using EventStateList = std::vector<EventState>;
    using FrontBufferMap = std::unordered_map<Chart::Channel, EventState*>;
    using InputStateMap  = std::unordered_map<Chart::Channel, bool>;
    using SoundMap       = std::unordered_map<unsigned int, sf::Sound*>;

    State* m_parent;
    NoteContainer* m_container;
    Gx::Node* m_menu;

    const Chart* m_chart;
    RenderSettings m_settings;
    JudgementStrategy* m_judgement;
    ScoreTracker* m_scores;

    ChannelSet m_instantiables;
    SpeedMap m_speeds;
    sf::Clock m_timer;
    mutable PrefabMap m_prefabs;
    mutable AnimationMap m_noteClicks;
    mutable AnimationMap m_longNoteEffects;
    mutable JudgementIndicator m_judgementIndicator;
    mutable ComboCounter m_comboCounter;
    mutable PlayMenu m_playMenu;
    mutable EventStateList m_events;
    mutable FrontBufferMap m_frontBuffers;
    mutable InputStateMap m_inputs;
    mutable SoundMap m_sounds;
    mutable double m_currentTime;
    mutable double m_refTime;
    mutable double m_refPosition;
    mutable double m_bpm;
    mutable unsigned int m_frameId;
};

#endif
