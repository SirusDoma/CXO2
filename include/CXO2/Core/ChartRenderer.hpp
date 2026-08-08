#pragma once

#include <CXO2/Core/Chart.hpp>
#include <CXO2/Core/JudgementStrategy.hpp>
#include <CXO2/Core/LifeSystem.hpp>

#include <CXO2/States/State.hpp>
#include <CXO2/Config/GameConfig.hpp>
#include <CXO2/Models/Game.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/Tasks/Delay.hpp>

#include <unordered_map>

#include <CXO2/Constants/Identifiers/Sound.hpp>
namespace Cx
{
    using namespace Constants::Identifiers;

    using NotePrefabMap = std::unordered_map<NoteShape, Gx::Sprite*>;

    using ChannelSet = std::unordered_set<Chart::Channel>;
    using SpeedMap = std::unordered_map<Chart::Channel, float>;

    class NoteContainer;
    class Equalizer;
    class ChartRenderer : public virtual Gx::Node, public Gx::RenderableContainer, public Gx::UpdatableContainer
    {
    public:
        struct RenderSettings
        {
            bool           Autoplay;
            GameConfig&    Config;
            unsigned int   Viewport;
            float          Speed;
            Cx::SpeedMode  SpeedMode;
            Cx::Difficulty Difficulty;

            // TODO: Modifiers
        };

        struct EventState
        {
            Chart::Event* Event;
            Chart::Event* LastEvent;
            Judgement     Tap     = {Accuracy::None, 0.f};
            Judgement     Release = {Accuracy::None, 0};

            bool IsRenderable(const double position) const;
            bool IsRegistered() const;

            Chart::Event* operator->() const { return Event; }
        };

        static constexpr unsigned int DefaultMeasureHeight = 384;

        ChartRenderer(
            JudgementStrategy& judgement,
            LifeSystem& life,
            ScoreTracker& scores,
            Gx::AudioMixer& mixer,
            Gx::ResourceManager& prefabResources,
            const ChannelSet& instantiables
        );
        ~ChartRenderer() override;

        void Initialize(const Chart& chart, const RenderSettings& settings);

        void StartRender();
        bool IsRendering() const;

        void Input(Chart::Channel channel, bool pressed) const;

        const RenderSettings& GetRenderSettings() const;
        float GetSpeed(Chart::Channel channel) const;
        double GetCurrentTime() const;
        double GetCurrentBPM() const;
        double GetRenderPosition() const;
        double GetLastMeasurePosition() const;
        std::unordered_map<Chart::Channel, EventState*> GetFrontBuffers() const;

        Equalizer* GetEqualizer() const;
        void SetEqualizer(Equalizer& equalizer) const;

        void SetRenderCompleteCallback(const std::function<void()> &completeCallback);
        void SetInputCallback(const std::function<void(Chart::Channel, bool)> &inputCallback);

        int MapRenderPositionToPixels(Chart::Channel channel, double position, bool absolute = false) const;

    protected:
        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

    private:
        // Measure interval per millisecond @ 60bpm in 1/4 note
        static constexpr double TickSignature = 60000.f * 4.f;

        void PlaySample(const Chart::NoteEvent* ev, const std::string& group = Sound::Channel::BGM) const;

        using AnimationMap   = std::unordered_map<Chart::Channel, Gx::Animation*>;
        using EventStateList = std::vector<EventState>;
        using FrontBufferMap = std::unordered_map<Chart::Channel, EventState*>;
        using InputStateMap  = std::unordered_map<Chart::Channel, bool>;
        using SoundMap       = std::unordered_map<unsigned int, sf::Sound*>;

        JudgementStrategy& m_judgement;
        LifeSystem& m_life;
        ScoreTracker& m_scores;
        Gx::AudioMixer& m_mixer;
        Gx::ResourceManager& m_prefabResources;
        ChannelSet m_instantiables;

        NoteContainer* m_container;
        bool m_rendering;
        double m_endPosition;

        const Chart* m_chart;
        mutable Gx::ResourceManager m_resources;
        std::unique_ptr<RenderSettings> m_settings;
        std::unordered_map<Chart::Channel, Gx::Delay> m_autoDelays;

        SpeedMap m_speeds;
        sf::Clock m_timer;
        mutable Equalizer* m_equalizer;
        EventStateList m_events;
        mutable FrontBufferMap m_frontBuffers;
        mutable InputStateMap m_inputs;
        mutable SoundMap m_sounds;
        double m_currentTime;
        double m_refTime;
        double m_refPosition;
        double m_bpm;
        double m_inputTime;
        unsigned int m_lastEventID;
        bool m_completed;

        std::function<void()> m_completeCallback;
        std::function<void(Chart::Channel, bool)> m_inputCallback;
    };
}
