#include <OTwo/Core/ChartRenderer.hpp>
#include <OTwo/Core/ScoreTracker.hpp>
#include <OTwo/Core/LifeSystem.hpp>
#include <OTwo/Core/Note.hpp>
#include <OTwo/Core/NoteContainer.hpp>
#include <OTwo/Core/NoteFactory.hpp>
#include <OTwo/Config/GameConfig.hpp>

#include <Genode/Tween/Fade.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/Utilities/Randomizer.hpp>
#include <OTwo/UI/Playing/Equalizer.hpp>

ChartRenderer::ChartRenderer(JudgementStrategy& judgement, LifeSystem& life, ScoreTracker& scores, Gx::AudioMixer& mixer, Gx::ResourceManager& prefabResources, const ChannelSet& instantiables) :
    m_judgement(judgement),
    m_life(life),
    m_scores(scores),
    m_mixer(mixer),
    m_prefabResources(prefabResources),
    m_instantiables(instantiables),
    m_container(),
    m_rendering(false),
    m_endPosition(),
    m_chart(),
    m_resources(),
    m_settings(),
    m_speeds(),
    m_timer(),
    m_equalizer(),
    m_events(),
    m_frontBuffers(),
    m_sounds(),
    m_currentTime(0),
    m_refTime(0),
    m_refPosition(0),
    m_bpm(0),
    m_inputTime(0),
    m_frameID(0),
    m_lastEventID(0),
    m_completed(false),
    m_completeCallback(),
    m_inputCallback()
{
}

ChartRenderer::~ChartRenderer()
{
    m_rendering = false;
    m_mixer.StopAll();
}

void ChartRenderer::Initialize(const Chart& chart, const GameContext& context)
{
    Initialize(chart, RenderSettings{
        false,
        context.GetConfig(),
        context.GetViewport(),
        context.GetSpeed(),
        context.GetDifficulty()
    });
}

void ChartRenderer::Initialize(const Chart& chart, const RenderSettings& settings)
{
    m_chart    = &chart;
    m_settings = std::make_unique<RenderSettings>(settings);

    // Setup judgement
    m_judgement.Initialize(*this);

    // Setup Speed
    const auto speed = settings.Speed;
    for (auto channel : Chart::NoteChannels)
    {
        if (speed == XrSpeed)
        {
            if (channel == Chart::Channel::Background)
                m_speeds[channel] = 1.0f;
            else
                m_speeds[channel] = SupportedHiSpeeds[Gx::Randomizer::Randomize(0, static_cast<int>(SupportedHiSpeeds.size()) - 1)];
        }
        else
            m_speeds[channel] = speed;

        if (m_instantiables.find(channel) != m_instantiables.end())
        {
            m_inputs[channel] = false;
            m_frontBuffers[channel] = nullptr;
        }
    }

    // Create Note Container with Note Factory
    const auto factory = NoteFactory(m_resources, m_prefabResources, m_instantiables);
    m_container = &factory.Generate(settings);
    m_container->SetName("IDC_NOTE_CONTAINER");
    m_container->Initialize(*this, chart, settings.Difficulty);
    if (const auto container = FindChild<NoteContainer>(m_container->GetName()); container)
        RemoveChild(*container);

    AddChild(*m_container);

    // Register events for processing
    m_events.clear();
    for (auto& ev : m_chart->GetEvents(settings.Difficulty))
        m_events.push_back(EventState{ ev.get() });

}

void ChartRenderer::StartRender()
{
    // Reset rendering states
    m_rendering   = true;
    m_currentTime = 0;
    m_refPosition = 0;
    m_refTime     = 0;
    m_frameID     = 0;
    m_lastEventID = 0;
    m_bpm         = m_chart->GetMetadata().BPM;
    m_endPosition = std::ceil(m_chart->GetLastEventPosition(m_settings->Difficulty)) + 1.f;
    m_timer.restart();
}

bool ChartRenderer::IsRendering() const
{
    return m_rendering;
}

Gx::RenderStates ChartRenderer::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
{
    if (!m_chart || !m_rendering)
    {
        if (m_instantiables.empty())
            return states;

        // Still send key press event with empty front buffers for fun
        const auto keyMode = static_cast<KeyMode>(m_instantiables.size());
        for (auto [channel, key] :  m_settings->Config.KeyBindings.at(keyMode))
            Input(channel, isKeyPressed(key));

        return states;
    }

    if (GetRenderPosition() > m_endPosition)
    {
        states = RenderableContainer::Render(surface, states);
        if (m_completeCallback && !m_completed)
        {
            m_completed = true;
            m_completeCallback();
        }

        return states;
    }

    // Skip multiple render calls
    if (states.FrameID == m_frameID)
        return RenderableContainer::Render(surface, states);

    // Save the current frame time so the generated render position is always consistent across multiple calls in the same frame
    m_currentTime = m_timer.getElapsedTime().asMilliseconds();
    m_frameID = states.FrameID;

    // Update input states
    if (!m_settings->Autoplay)
    {
        // TODO: Implement poll rate
        m_inputTime = m_currentTime;
        const auto keyMode = static_cast<KeyMode>(m_instantiables.size());
        for (auto [channel, key] : m_settings->Config.KeyBindings.at(keyMode))
            Input(channel, isKeyPressed(key));
    }
    else
    {
        for (auto& [channel, state] : m_frontBuffers)
        {
            static_cast<Gx::Updatable*>(&m_autoDelays[channel])->Update(states.Delta);
            if (!state || state->IsRegistered())
            {
                Input(channel, false);
                continue;
            }

            auto& ev = static_cast<Chart::NoteEvent&>(*state->Event);
            if (state->Tap.Accuracy != Accuracy::None && ev.Length > 0)
            {
                auto release = Chart::NoteEvent(ev);
                release.Type = Chart::NoteType::Release;
                release.Position += ev.Length;
                if (const auto releaseResult = m_judgement.Judge(release); releaseResult.Latency <= 0)
                {
                    Input(channel, false);
                }
            }
            else if (const auto tapResult = m_judgement.Judge(ev); state->Tap.Accuracy == Accuracy::None && tapResult.Latency <= 0)
            {
                if (m_inputs[channel])
                {
                    m_autoDelays[channel].Stop();
                    Input(channel, false);
                }

                Input(channel, true);
                if (ev.Length == 0)
                    m_autoDelays[channel] = Gx::Delay(sf::seconds(1.f / 60.f * 5.f), [this, c = channel] { Input(c, false); });
            }
        }
    }

    // Check judgement status of front buffer events
    bool frontBuffersFilled = true;
    for (auto& [channel, state] : m_frontBuffers)
    {
        if (!state || state->IsRegistered())
        {
            if (!state)
            {
                m_lastEventID = 0;
                frontBuffersFilled = false;
            }

            continue;
        }

        auto& ev = static_cast<Chart::NoteEvent&>(*state->Event);
        if (const auto tapResult = m_judgement.Judge(ev); state->Tap.Accuracy == Accuracy::None && tapResult.Accuracy == Accuracy::Miss)
        {
            state->Tap = tapResult;
            if (ev.Length > 0)
                state->Release = tapResult;

            m_scores.Increment(ev, tapResult.Accuracy, ev.Length > 0 ? 2 : 1);
        }
        else if (ev.Length > 0)
        {
            auto release = Chart::NoteEvent(ev);
            release.Type = Chart::NoteType::Release;
            release.Position += ev.Length;
            if (const auto releaseResult = m_judgement.Judge(release); releaseResult.Accuracy == Accuracy::Miss)
            {
                state->Release = releaseResult;
                m_scores.Increment(ev, releaseResult.Accuracy);
            }
        }
    }

    bool updated = false;
    for (unsigned int i = m_lastEventID; i < m_events.size(); i++)
    {
        auto& ev = m_events[i];
        const double latency = ev->Position - GetRenderPosition();

        if (!ev.IsRenderable(GetRenderPosition()))
            continue;

        if ((i == m_events.size() - 1 || m_events[i + 1]->Position > ev->Position) && latency >= 3.f && frontBuffersFilled)
            break;

        if (!updated)
        {
            m_lastEventID = i;
            updated = true;
        }

        // Fill front buffers and check for misses
        if (ev->IsPlayable() && m_instantiables.find(ev->Channel) != m_instantiables.end())
        {
            // Fill buffer with valid event
            const auto& note = static_cast<const Chart::NoteEvent&>(*ev.Event);
            if (const auto front = m_frontBuffers[ev->Channel]; !front || front->IsRegistered())
            {
                m_frontBuffers[ev->Channel] = &ev;
                if (front)
                   ev.LastEvent = front->Event;

                frontBuffersFilled = frontBuffersFilled || std::all_of(m_frontBuffers.begin(), m_frontBuffers.end(), [this] (const auto& b)
                {
                    return m_instantiables.find(b.first) == m_instantiables.end() || b.second;
                });
            }

            m_container->UpdateGeometry(note, states.Delta);
        }
        else
        {
            // Ignore non-playable events that are not inside perfect line
            if (ev.Tap.Accuracy != Accuracy::None || latency > 0)
                continue;

            ev.Tap = Judgement{Accuracy::Cool, 0};
            if (ev->Channel == Chart::Channel::BPM)
            {
                const auto time = static_cast<Chart::TimeEvent*>(ev.Event);
                if (std::abs(time->Value - m_bpm) < 0.f)
                    continue;

                m_refTime     += (time->Position - m_refPosition) / m_bpm * TickSignature;
                m_refPosition  = time->Position;
                m_bpm          = time->Value;
            }
            else if (ev->Channel != Chart::Channel::Measure)
            {
                if (const auto bgm = static_cast<Chart::NoteEvent*>(ev.Event); bgm)
                    PlaySample(bgm, bgm->SampleType == Chart::SampleType::Background ? "BGM" : "SFX");
            }
        }
    }

    if (m_equalizer)
        m_equalizer->Update(states.Delta);

    return RenderableContainer::Render(surface, states);
}

void ChartRenderer::Input(const Chart::Channel channel, const bool pressed) const
{
    if (const auto it = m_inputs.find(channel); it != m_inputs.end() && it->second == pressed)
        return;

    m_inputs[channel] = pressed;
    if (const auto front = m_frontBuffers[channel]; front)
    {
        const auto note = static_cast<Chart::NoteEvent*>(front->Event);
        if (pressed)
        {
            auto result = m_judgement.Judge(*note);
            if (front->Tap.Accuracy == Accuracy::None && result.Accuracy != Accuracy::None)
            {
                result.Accuracy = m_scores.Increment(*note, result.Accuracy);
                front->Tap      = result;
                if (note->Length > 0 && result.Accuracy == Accuracy::Bad && m_scores.GetBufferCount() <= 0)
                {
                    auto release      = Chart::NoteEvent(*note);
                    release.Type      = Chart::NoteType::Release;
                    release.Position += note->Length;

                    front->Release = Judgement{ Accuracy::Miss, 0.f };
                    m_scores.Increment(release, Accuracy::Miss);
                }
            }

            if (front->LastEvent && front->Tap.Accuracy == Accuracy::None)
                PlaySample(static_cast<Chart::NoteEvent*>(front->LastEvent), "SFX");
            else
                PlaySample(note, "SFX");
        }
        else if (note->Length > 0 && front->Tap.Accuracy != Accuracy::None)
        {
            auto release      = Chart::NoteEvent(*note);
            release.Type      = Chart::NoteType::Release;
            release.Position += note->Length;

            if (auto result = m_judgement.Judge(release); front->Release.Accuracy == Accuracy::None)
            {
                if (result.Accuracy == Accuracy::None)
                    result.Accuracy = Accuracy::Miss;

                result.Accuracy = m_scores.Increment(release, result.Accuracy);
                front->Release  = result;

            }
        }
    }

    if (m_inputCallback)
        m_inputCallback(channel, pressed);
}

const ChartRenderer::RenderSettings& ChartRenderer::GetRenderSettings() const
{
    return *m_settings;
}

float ChartRenderer::GetSpeed(const Chart::Channel channel) const
{
    if (const auto it = m_speeds.find(channel); it != m_speeds.end())
        return it->second;

    return 1.0f;
}

double ChartRenderer::GetCurrentTime() const
{
    return m_currentTime;
}

double ChartRenderer::GetCurrentBPM() const
{
    return m_bpm;
}

double ChartRenderer::GetRenderPosition() const
{
    return ((m_currentTime - m_refTime) / TickSignature* m_bpm) + m_refPosition;
}

double ChartRenderer::GetLastMeasurePosition() const
{
    return m_endPosition;
}

std::unordered_map<Chart::Channel, ChartRenderer::EventState*> ChartRenderer::GetFrontBuffers() const
{
    return m_frontBuffers;
}

Equalizer* ChartRenderer::GetEqualizer() const
{
    return m_equalizer;
}

void ChartRenderer::SetEqualizer(Equalizer& equalizer) const
{
    m_equalizer = &equalizer;
}

void ChartRenderer::SetRenderCompleteCallback(const std::function<void()>& completeCallback)
{
    m_completeCallback = completeCallback;
}

void ChartRenderer::SetInputCallback(const std::function<void(Chart::Channel, bool)> &inputCallback)
{
    m_inputCallback = inputCallback;
}

int ChartRenderer::MapRenderPositionToPixels(const Chart::Channel channel, const double position, const bool absolute) const
{
    float speed = 1.0f;
    if (const auto it = m_speeds.find(channel); it != m_speeds.end())
        speed = it->second;

    const int pixels = static_cast<int>(position * (static_cast<float>(DefaultMeasureHeight) * speed));
    return absolute ? pixels : static_cast<int>(m_settings->Viewport) - pixels;
}

void ChartRenderer::PlaySample(const Chart::NoteEvent* ev, const std::string& group) const
{
    if (!ev || !ev->Sample)
        return;

    if (m_sounds.find(ev->ID) == m_sounds.end())
    {
        m_sounds[ev->ID] = &m_resources.Create<sf::Sound>(std::to_string(ev->ID), *ev->Sample);
        m_sounds[ev->ID]->setVolume(ev->Volume);
    }

    if (m_equalizer && ev->SampleType == Chart::SampleType::KeySound && ev->Sample->getDuration() < sf::seconds(60.0))
        m_equalizer->Register(*ev, *m_sounds[ev->ID]);

    m_mixer.Play(*m_sounds[ev->ID], group);
}

bool ChartRenderer::EventState::IsRenderable(const double position) const
{
    if (!Event)
        return false;

    if (!Event->IsPlayable())
        return Tap.Accuracy == Accuracy::None;

    const auto note = static_cast<Chart::NoteEvent*>(Event);
    const double threshold = position - 0.25f;
    return (note->Length == 0 && note->Position > threshold && Tap.Accuracy == Accuracy::None) ||
           (note->Length > 0  && note->Position + note->Length > threshold);
}

bool ChartRenderer::EventState::IsRegistered() const
{
    if (!Event)
        return true;

    if (!Event->IsPlayable() && Tap.Accuracy != Accuracy::None)
        return true;

    const auto note = static_cast<Chart::NoteEvent*>(Event);
    return Tap.Accuracy != Accuracy::None && (note->Length == 0 || Release.Accuracy != Accuracy::None);
}
