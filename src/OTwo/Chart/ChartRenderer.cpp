#include <OTwo/Chart/ChartRenderer.hpp>
#include <OTwo/Config/GameConfig.hpp>

#include <OTwo/Chart/NoteContainer.hpp>
#include <OTwo/Chart/NoteFactory.hpp>

#include <Genode/Utilities/Randomizer.hpp>

ChartRenderer::ChartRenderer(State &state, const ChannelSet &instantiables) :
    m_parent(&state),
    m_container(),
    m_chart(),
    m_settings(),
    m_instantiables(instantiables),
    m_speeds(),
    m_sounds(),
    m_timer(),
    m_prefabs(),
    m_events(),
    m_frontBuffers(),
    m_currentTime(0),
    m_refTime(0),
    m_refPosition(0),
    m_bpm(0),
    m_frameId(0)
{
}

void ChartRenderer::Render(const Chart &chart, const GameContext &context)
{
    if (!context.GetConfig())
        throw Gx::Exception("GameConfig cannot be null");

    Render(chart, RenderSettings{
        context.GetConfig(),
        context.GetViewport(),
        context.GetSpeed(),
        context.GetDifficulty()
    });
}

void ChartRenderer::Render(const Chart &chart, const RenderSettings &settings)
{
    if (!settings.Config)
        throw Gx::Exception("GameConfig cannot be null");

    m_chart    = &chart;
    m_settings = settings;

    // Set-up Speed
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
    const auto factory = NoteFactory(
        m_parent->GetResources(ResourceScope::Immediate),
        m_parent->GetResources(ResourceScope::Local),
        m_instantiables
    );
    m_container = factory.Generate(chart, settings);

    // Register events for processing
    m_events.clear();
    for (Chart::Event *ev : m_chart->GetEvents(settings.Difficulty))
        m_events.push_back(EventState{ ev });

    // Set-up rendering states
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

    // Save the current frame time so the generated render position is always consistent across multiple calls in the same frame
    if (states.FrameID != m_frameId)
    {
        m_currentTime = m_timer.getElapsedTime().asMilliseconds();
        m_frameId = states.FrameID;
    }

    // Update note animation
    for (const auto updatable : m_container->GetRegisteredPrefabs())
        updatable->Update(states.Delta);

    // TODO: Gameplay
    for (auto &ev : m_events)
    {
        if (ev.Accuracy != Accuracy::None)
            continue;

        const double latency = ev->Position - GetRenderPosition();
        if (!ev->IsPlayable())
        {
            if (latency > 0)
                continue;

            ev.Accuracy = Accuracy::Cool;
            ev.Latency  = latency;

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
                if (const auto bgm = static_cast<Chart::NoteEvent*>(ev.Event); bgm->Sample)
                {
                    auto& mixer = m_parent->GetApplication().Require<Gx::Mixer>();
                    const auto sound = m_parent->Create<sf::Sound>(*bgm->Sample);
                    mixer.Play(sound, "BGM");
                }
            }
        }
        else
        {
            if (const auto front = m_frontBuffers[ev->Channel]; !front || latency < 1.f / 16.f)
                m_frontBuffers[ev->Channel] = &ev;

            if (latency > 0)
                continue;

            if (latency < -0.5f)
            {
                ev.Accuracy = Accuracy::Miss;
                ev.Latency  = latency;
            }
        }
    }

    m_container->Render(*this, states.Delta);
    surface.Render(*m_container, states);

    return states;
}

void ChartRenderer::Input(const Chart::Channel channel, const bool pressed)
{
    if (const auto it = m_inputs.find(channel); it != m_inputs.end() && it->second == pressed)
        return;

    m_inputs[channel] = pressed;
    if (const auto front = m_frontBuffers[channel]; front)
    {
        const auto note = static_cast<Chart::NoteEvent*>(front->Event);
        if (!pressed || !note->Sample)
            return;

        auto& mixer = m_parent->GetApplication().Require<Gx::Mixer>();
        if (m_sounds.find(note->ID) == m_sounds.end())
            m_sounds[note->ID] = m_parent->Create<sf::Sound>(*note->Sample);

        mixer.Play(m_sounds[note->ID], "BGM");
        if (note->Position - GetRenderPosition() < 1.f / 16.f)
        {
            m_container->GetNote(front->Event->Channel, front->Event->Position)->Hit();

            front->Accuracy = Accuracy::Cool;
            front->Latency  = 0;
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

int ChartRenderer::MapRenderPositionToPixels(const Chart::Channel channel, const double position, const bool absolute) const
{
    float speed = 1.0f;
    if (const auto it = m_speeds.find(channel); it != m_speeds.end())
        speed = it->second;

    const unsigned int pixels = (position * (static_cast<float>(DefaultMeasureHeight) * speed));
    return absolute ? pixels : m_settings.Viewport - pixels;
}
