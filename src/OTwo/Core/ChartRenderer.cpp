
#include <OTwo/Core/ChartRenderer.hpp>
#include <OTwo/Config/GameConfig.hpp>

#include <OTwo/Core/NoteContainer.hpp>
#include <OTwo/Core/NoteFactory.hpp>
#include <OTwo/Core/JudgementStrategy.hpp>

#include <Genode/Fx/Fade.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/Utilities/Randomizer.hpp>

ChartRenderer::ChartRenderer(const ChannelSet &instantiables) :
    m_container(),
    m_menu(),
    m_chart(),
    m_settings(),
    m_judgement(),
    m_scores(),
    m_instantiables(instantiables),
    m_speeds(),
    m_timer(),
    m_prefabs(),
    m_noteClicks(),
    m_longNoteEffects(),
    m_judgementIndicator(),
    m_comboCounter(),
    m_playMenu(),
    m_events(),
    m_frontBuffers(),
    m_sounds(),
    m_currentTime(0),
    m_refTime(0),
    m_refPosition(0),
    m_bpm(0),
    m_frameId(0),
    m_callbackCalled(false),
    m_callback(),
    m_incrementCallback(),
    m_jamComboCallback()
{
}

void ChartRenderer::Render(const Chart &chart, const GameContext &context, const std::function<void()>& callback)
{
    if (!context.GetConfig())
        throw Gx::Exception("GameConfig cannot be null");

    Render(chart, RenderSettings{
        context.GetConfig(),
        context.GetViewport(),
        context.GetSpeed(),
        context.GetDifficulty()
    }, callback);
}

void ChartRenderer::Render(const Chart &chart, const RenderSettings &settings, const std::function<void()>& callback)
{
    const auto parent = GetParent<State>();
    if (!parent)
        throw Gx::Exception("ChartRenderer needs to be attached into a State!");
    
    if (!settings.Config)
        throw Gx::Exception("GameConfig cannot be null");

    m_chart    = &chart;
    m_settings = settings;
    m_callback = callback;

    // Setup judgement
    m_judgement = &parent->Require<JudgementStrategy>();
    m_judgement->Initialize(*this);

    // Setup score tracker
    m_scores = &parent->Require<ScoreTracker>();
    m_scores->Initialize(settings.Difficulty);
    m_scores->SetIncrementCallback([this] (auto ev, auto acc, auto count) {
        OnScoreUpdated(ev, acc, count);
        if (m_incrementCallback)
            m_incrementCallback(ev, acc, count);
    });

    m_scores->SetJamComboCallback([this] (auto ev, auto acc, auto count) {
        if (m_jamComboCallback)
            m_jamComboCallback(ev, acc, count);
    });

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
            m_noteClicks[channel] = nullptr;
        }
    }

    // Create Note Container with Note Factory
    const auto factory = NoteFactory(
        parent->GetResources(ResourceScope::Immediate),
        parent->GetResources(ResourceScope::Local),
        m_instantiables
    );
    RemoveChild(FindChild<NoteContainer>("IDC_NOTE_CONTAINER"));
    m_container = factory.Generate(chart, settings);
    m_container->SetName("IDC_NOTE_CONTAINER");
    AddChild(m_container);

    // Setup menu
    RemoveChild(FindChild<PlayMenu>("IDC_PLAY_MENU"));
    m_playMenu = PlayMenu();
    m_playMenu.SetName("IDC_PLAY_MENU");
    AddChild(&m_playMenu);
    m_playMenu.Initialize();
    m_playMenu.SetMetadata(chart.GetMetadata().ToChartMetadataView(settings.Difficulty), settings.Difficulty);
    m_playMenu.SetScoreTracker(*m_scores);

    // Setup Combo Animation
    RemoveChild(FindChild<ComboCounter>("IDC_CONTAINER_COMBO"));
    m_comboCounter = ComboCounter();
    m_comboCounter.SetName("IDC_CONTAINER_COMBO");
    AddChild(&m_comboCounter);
    m_comboCounter.Initialize();

    // Setup Judgement Indicator
    RemoveChild(FindChild<JudgementIndicator>("IDC_NOTE_JUDGEMENT_INDICATOR"));
    m_judgementIndicator = JudgementIndicator();
    m_judgementIndicator.SetName("IDC_NOTE_JUDGEMENT_INDICATOR");
    AddChild(&m_judgementIndicator);
    m_judgementIndicator.Initialize();

    // Setup Note Clicks
    const auto keyMode = static_cast<KeyMode>(m_instantiables.size());
    if (const auto noteClickList = parent->FindResource<Gx::List>("IDC_LIST_NOTE_CLICK"); noteClickList)
    {
        for (auto [channel, _] : settings.Config->KeyBindings.at(keyMode))
        {
            const int id = static_cast<int>(channel) - 1;
            if (id < 1 || id > 7)
                continue;

            const auto noteClick = noteClickList->FindChild<Gx::Animation>("IDC_ANIMATION_NOTE_CLICK" + std::to_string(id));
            noteClick->SetVisible(false);
            noteClick->Stop();
            noteClick->SetAnimationCallback([] (auto &animation) {
                animation.SetVisible(animation.GetState() == Gx::Animation::AnimationState::Playing);
            });

            m_noteClicks[channel] = noteClick;
            AddChild(noteClick);
        }
    }

    if (const auto longNoteEffectList = parent->FindResource<Gx::List>("IDC_LIST_LONG_NOTE_EFFECT"); longNoteEffectList)
    {
        for (auto [channel, _] : settings.Config->KeyBindings.at(keyMode))
        {
            const int id = static_cast<int>(channel) - 1;
            if (id < 1 || id > 7)
                continue;

            const auto longNoteEffect = longNoteEffectList->FindChild<Gx::Animation>("IDC_ANIMATION_LONG_NOTE_EFFECT" + std::to_string(id));
            longNoteEffect->SetVisible(false);
            m_longNoteEffects[channel] = longNoteEffect;

            AddChild(longNoteEffect);
        }
    }

    // Register events for processing
    m_events.clear();
    for (Chart::Event *ev : m_chart->GetEvents(settings.Difficulty))
        m_events.push_back(EventState{ ev });

    // Reset rendering states
    m_currentTime = 0;
    m_refPosition = 0;
    m_refTime     = 0;
    m_frameId     = 0;
    m_bpm         = chart.GetMetadata().BPM;
    m_timer.restart();
}

Gx::RenderStates ChartRenderer::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    if (!m_chart)
        return states;

    if (GetRenderPosition() > m_container->GetLastMeasure())
    {
        m_container->Render(*this, states.Delta);
        states = RenderableContainer::Render(surface, states);

        if (m_callback && !m_callbackCalled)
        {
            m_callbackCalled = true;
            m_callback();
        }

        return states;
    }

    // Skip multiple render calls
    if (states.FrameID == m_frameId)
        return RenderableContainer::Render(surface, states);

    // Save the current frame time so the generated render position is always consistent across multiple calls in the same frame
    m_currentTime = m_timer.getElapsedTime().asMilliseconds();
    m_frameId = states.FrameID;

    // Update input states
    // TODO: Implement poll rate
    m_inputTime = m_currentTime;
    const auto keyMode = static_cast<KeyMode>(m_instantiables.size());
    for (auto [channel, key] : m_settings.Config->KeyBindings.at(keyMode))
        Input(channel, isKeyPressed(key));

    // Check judgement status of front buffer events
    for (auto& [channel, state] : m_frontBuffers)
    {
        if (!state || state->IsRegistered())
            continue;

        auto& ev = static_cast<Chart::NoteEvent&>(*state->Event);
        if (const auto tapResult = m_judgement->Judge(ev); state->Tap.Accuracy == Accuracy::None && tapResult.Accuracy == Accuracy::Miss)
        {
            state->Tap = tapResult;
            if (ev.Length > 0)
                state->Release = tapResult;

            m_scores->Increment(ev, tapResult.Accuracy, ev.Length > 0 ? 2 : 1);
        }
        else if (ev.Length > 0)
        {
            auto release = Chart::NoteEvent(ev);
            release.Type = Chart::NoteType::Release;
            release.Position += ev.Length;
            if (const auto releaseResult = m_judgement->Judge(release); releaseResult.Accuracy == Accuracy::Miss)
            {
                state->Release = releaseResult;
                m_scores->Increment(ev, releaseResult.Accuracy);
            }
        }
    }

    for (auto &ev : m_events)
    {
        if (ev.Tap.Accuracy != Accuracy::None)
            continue;

        // Fill front buffers and check for misses
        if (ev->IsPlayable())
        {
            auto& note = static_cast<Chart::NoteEvent&>(*ev.Event);
            const auto result = m_judgement->Judge(note);

            // Fill buffer with valid event
            if (const auto front = m_frontBuffers[ev->Channel]; !front || front->IsRegistered())
            {
                m_frontBuffers[ev->Channel] = &ev;
                if (front)
                   ev.LastEvent = front->Event;
            }
        }
        else
        {
            // Ignore non playable events that are not inside perfect line
            if (const double latency = ev->Position - GetRenderPosition(); latency > 0)
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
            else if (ev->Channel == Chart::Channel::Background)
            {
                if (const auto bgm = static_cast<Chart::NoteEvent*>(ev.Event); bgm)
                    PlaySample(bgm, "BGM");
            }
        }
    }

    m_container->Render(*this, states.Delta);
    return RenderableContainer::Render(surface, states);;
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
            auto result = m_judgement->Judge(*note);
            if (front->Tap.Accuracy == Accuracy::None && result.Accuracy != Accuracy::None)
            {
                result.Accuracy = m_scores->Increment(*note, result.Accuracy);
                front->Tap      = result;
                if (note->Length > 0 && result.Accuracy == Accuracy::Bad && m_scores->GetBufferCount() <= 0)
                {
                    auto release      = Chart::NoteEvent(*note);
                    release.Type      = Chart::NoteType::Release;
                    release.Position += note->Length;

                    front->Release = Judgement{ Accuracy::Miss, 0.f };
                    m_scores->Increment(release, Accuracy::Miss);
                }
                else
                    m_container->GetNote(front->Event->Channel, front->Event->Position)->Hit();
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

            if (auto result = m_judgement->Judge(release); front->Release.Accuracy == Accuracy::None)
            {
                if (result.Accuracy == Accuracy::None)
                    result.Accuracy = Accuracy::Miss;

                result.Accuracy = m_scores->Increment(release, result.Accuracy);
                front->Release  = result;

            }
        }
    }
}

const ChartRenderer::RenderSettings &ChartRenderer::GetRenderSettings() const
{
    return m_settings;
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

double ChartRenderer::GetRenderPosition() const
{
    return ((m_currentTime - m_refTime) / TickSignature * m_bpm) + m_refPosition;
}

double ChartRenderer::GetCurrentBPM() const
{
    return m_bpm;
}

void ChartRenderer::SetIncrementCallback(const std::function<void(const Chart::NoteEvent&, Accuracy, unsigned int)>& incrementCallback)
{
    m_incrementCallback = incrementCallback;
}

void ChartRenderer::SetJamComboCallback(const std::function<void(const Chart::NoteEvent&, Accuracy, unsigned int)>& jamComboCallback)
{
    m_jamComboCallback = jamComboCallback;
}

int ChartRenderer::MapRenderPositionToPixels(const Chart::Channel channel, const double position, const bool absolute) const
{
    float speed = 1.0f;
    if (const auto it = m_speeds.find(channel); it != m_speeds.end())
        speed = it->second;

    const unsigned int pixels = (position * (static_cast<float>(DefaultMeasureHeight) * speed));
    return absolute ? pixels : m_settings.Viewport - pixels;
}

void ChartRenderer::PlaySample(const Chart::NoteEvent *ev, const std::string &group) const
{
    if (!ev || !ev->Sample)
        return;

    const auto parent = GetParent<State>();
    auto& mixer = parent->GetApplication().Require<Gx::Mixer>();
    if (m_sounds.find(ev->ID) == m_sounds.end())
    {
        m_sounds[ev->ID] = parent->Create<sf::Sound>(*ev->Sample);
        m_sounds[ev->ID]->setVolume(ev->Volume);
    }

    mixer.Play(m_sounds[ev->ID], group);
}

void ChartRenderer::OnScoreUpdated(const Chart::NoteEvent& ev, const Accuracy acc, unsigned int count) const
{
    m_judgementIndicator.Play(acc);
    m_comboCounter.SetCombo(m_scores->GetCombo());

    m_noteClicks[ev.Channel]->Reset();
    m_longNoteEffects[ev.Channel]->SetVisible(false);

    if (acc == Accuracy::Bad || acc == Accuracy::Miss)
    {
        m_longNoteEffects[ev.Channel]->SetVisible(false);
        m_noteClicks[ev.Channel]->Stop();
    }
    else if (ev.Type == Chart::NoteType::Hold)
    {
        m_longNoteEffects[ev.Channel]->Reset();
        m_longNoteEffects[ev.Channel]->SetVisible(true);
    }
}
