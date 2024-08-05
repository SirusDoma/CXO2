#include <iostream>
#include <Genode/Utilities/Randomizer.hpp>
#include <OTwo/Chart/ChartRenderer.hpp>
#include <OTwo/Config/GameConfig.hpp>
#include <OTwo/Chart/Note.hpp>
#include <OTwo/Chart/LongNote.hpp>

ChartRenderer::ChartRenderer(State &state, const std::initializer_list<Chart::Channel> instantiables) :
    m_parent(&state),
    m_chart(),
    m_container(),
    m_settings(),
    m_instantiables(instantiables),
    m_prefabs(),
    m_speeds(),
    m_position(0),
    m_start(0),
    m_elapsed(0),
    m_reference(0),
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

    // Setup note container
    // TODO: CREATE NOTE-FACTORY CLASS, USE VERTEX BUFFER AND STATIC RENDER BATCHING
    if (!m_container)
    {
        if (m_container = m_parent->FindChild<Gx::RenderBatchContainer>("IDC_CONTAINER_NOTE"); !m_container)
        {
            m_container = m_parent->Create<Gx::RenderBatchContainer>();
            m_parent->AddChild(m_container);
        }
    }

    // Set-up Speed
    const auto speed = settings.Speed;
    for (auto channel : Chart::NoteChannels)
    {
        if (speed == XrSpeed)
        {
            if (channel == Chart::Channel::BGM)
                m_speeds[channel] = 1.0f;
            else
                m_speeds[channel] = SupportedHiSpeeds[Gx::Randomizer::Randomize(0, static_cast<int>(SupportedHiSpeeds.size()) - 1)];
        }
        else
            m_speeds[channel] = speed;
    }

    // Load notes templates
    for (auto channel : m_instantiables)
    {
        const int key = static_cast<Gx::Uint16>(channel) - 1;
        if (key <= 0 || key >= 8)
            continue;

        m_prefabs[channel] = {};
        m_prefabs[channel][Chart::NoteType::Normal] = {
            { NoteShape::Square, m_parent->FindResource<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_NORMAL" + std::to_string(key) + "_1") },
            { NoteShape::Circle, m_parent->FindResource<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_NORMAL" + std::to_string(key) + "_2") },
        };
        m_prefabs[channel][Chart::NoteType::Hold] = {
            { NoteShape::Square, m_parent->FindResource<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_LONG" + std::to_string(key) + "_1") },
            { NoteShape::Circle, m_parent->FindResource<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_LONG" + std::to_string(key) + "_2") },
        };
    }

    // Set-up rendering states
    m_position  = 0;
    m_start     = 0;
    m_elapsed   = 0;
    m_reference = 0;
    m_bpm       = chart.GetMetadata().BPM;

    const auto events = chart.GetEvents(settings.Difficulty);

    // Instantiate measure
    const auto max = *std::max_element(events.begin(), events.end(), [] (auto a, auto b) { return a->Position < b->Position; });
    NoteSpriteMap measure = {
        { NoteShape::Square, m_parent->FindResource<Gx::Sprite>("STATE_PLAYING/IDC_IMAGE_NOTE_MEASURE") },
        { NoteShape::Circle, m_parent->FindResource<Gx::Sprite>("STATE_PLAYING/IDC_IMAGE_NOTE_MEASURE") }
    };
    for (int i = 0; i < std::ceil(max->Position) + 1; i++)
    {
        const auto node = m_parent->Create<Note>(*this, Chart::Channel::BGM, static_cast<double>(i), measure);
        m_container->AddChild(node);
    }

    // Instantiate note objects
    m_events.clear();
    for (Chart::Event *ev : events)
    {
        m_events.push_back(EventState{ ev });
        if (ev->IsPlayable())
        {
            if (const auto note  = static_cast<Chart::NoteEvent*>(ev); note->Type == Chart::NoteType::Normal)
            {
                const auto node = m_parent->Create<Note>(*this, *note, m_prefabs[note->Channel][note->Type]);
                m_container->AddChild(node);
            }
            else if (note->Type == Chart::NoteType::Hold)
            {
                const auto node = m_parent->Create<LongNote>(*this, *note, m_prefabs[note->Channel][note->Type], m_prefabs[note->Channel][Chart::NoteType::Normal]);
                m_container->AddChild(node);
            }
        }
    }
}

Gx::RenderStates ChartRenderer::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    if (!m_chart)
        return states;

    // TODO: FIX ELAPSED NOT TO USE DELTA!
    if (states.FrameID != m_frameId)
        m_elapsed += states.Delta;

    m_frameId  = states.FrameID;
    m_position = m_reference + ((m_elapsed - m_start) / (60000 / m_bpm) / 4);

    // Update note animation
    for (auto channel : m_instantiables)
    {
        for (auto type : { Chart::NoteType::Normal, Chart::NoteType::Hold })
        {
            for (auto shape : { NoteShape::Square, NoteShape::Circle })
            {
                const auto instance = static_cast<Gx::Updatable*>(m_prefabs[channel][type][shape]);
                instance->Update(states.Delta);
            }
        }
    }

    // TODO: Gameplay
    for (auto &ev : m_events)
    {
        const double position = ev->Position - m_position;
        if (ev.Accuracy != Accuracy::None)
            continue;

        if (!ev->IsPlayable())
        {
            if (position > 0)
                continue;

            ev.Accuracy = Accuracy::Cool;
            ev.Latency  = position;

            if (ev->Channel == Chart::Channel::BPM)
            {
                const auto time = static_cast<Chart::TimeEvent*>(ev.Event);
                m_reference = m_position;
                m_start     = m_elapsed;
                m_bpm       = time->Value;

                break;
            }
            else if (ev->Channel == Chart::Channel::BGM)
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
            if (position > 0)
                continue;

            if (const auto note = static_cast<Chart::NoteEvent*>(ev.Event); note->Sample)
            {
                auto& mixer = m_parent->GetApplication().Require<Gx::Mixer>();
                const auto sound = m_parent->Create<sf::Sound>(*note->Sample);
                mixer.Play(sound, "BGM");

                ev.Accuracy = Accuracy::Cool;
                ev.Latency  = position;
            }
        }
    }


    auto va = sf::VertexArray(sf::PrimitiveType::Lines, 3);
    va.append({ sf::Vector2f(300, -5.5f), sf::Color::Black });
    va.append({ sf::Vector2f(300, 0.f), sf::Color::White });
    va.append({ sf::Vector2f(300, 5.5f), sf::Color::Black });

    surface.Render(va, states);

    return states;
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

double ChartRenderer::GetRenderPosition() const
{
    return m_position;
}

double ChartRenderer::GetBPM() const
{
    return m_bpm;
}

bool ChartRenderer::InRenderProximity(const double position) const
{
    const double distance = position - m_position;
    return distance < std::ceil(m_settings.Viewport / DefaultMeasureHeight) + 0.1f || distance > -0.1f;
}

int ChartRenderer::MapRenderPositionToPixels(const Chart::Channel channel, const double position, const bool relative) const
{
    float speed = 1.0f;
    if (const auto it = m_speeds.find(channel); it != m_speeds.end())
        speed = it->second;

    const unsigned int pixels = (position * (static_cast<float>(DefaultMeasureHeight) * speed));
    return relative ? pixels : m_settings.Viewport - pixels;
}
