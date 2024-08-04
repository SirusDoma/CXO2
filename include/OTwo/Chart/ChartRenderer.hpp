#ifndef O2JAM_CHART_RENDERER_HPP
#define O2JAM_CHART_RENDERER_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/Chart/Chart.hpp>
#include <OTwo/Contexts/GameContext.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Updatable.hpp>
#include <Genode/Graphics/Animation.hpp>

#include <unordered_map>

using NoteSpriteMap = std::unordered_map<NoteShape, Gx::Sprite*>;

class ChartRenderer : public Gx::Renderable
{
public:
    static constexpr unsigned int DefaultMeasureHeight = 384;

    explicit ChartRenderer(State &state, std::initializer_list<Chart::Channel> instantiables = {});

    void Initialize(const GameContext &context);
    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;

    const GameContext &GetContext() const;
    float GetSpeed(Chart::Channel channel) const;
    double GetRenderPosition() const;

private:
    struct EventState
    {
        Chart::Event *Event;
        Accuracy      Accuracy = Accuracy::None;
        double        Latency  = 0;

        Chart::Event *operator->() const { return Event; }
    };

    using PrefabMap      = std::unordered_map<Chart::Channel, std::unordered_map<Chart::NoteType, NoteSpriteMap>>;
    using EventStateList = std::vector<EventState>;

    State *m_parent;
    Gx::UiContainer *m_container;
    const GameContext *m_context;

    std::vector<Chart::Channel> m_instantiables;
    std::unordered_map<Chart::Channel, float> m_speeds;

    mutable PrefabMap m_prefabs;
    mutable EventStateList m_events;
    mutable double m_position;
    mutable double m_start;
    mutable double m_elapsed;
    mutable double m_reference;
    mutable double m_bpm;
    mutable unsigned int m_frameId;
};

#endif
