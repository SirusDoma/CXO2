#include <iostream>
#include <Genode/Utilities/Randomizer.hpp>
#include <OTwo/Chart/ChartRenderer.hpp>
#include <OTwo/Config/GameConfig.hpp>
#include <OTwo/Chart/Note.hpp>
#include <OTwo/Chart/LongNote.hpp>

ChartRenderer::ChartRenderer(State &state, const std::initializer_list<Chart::Channel> instantiables) :
    m_parent(&state),
    m_container(),
    m_context(),
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

void ChartRenderer::Initialize(const GameContext &context)
{
    m_context   = &context;
    m_container = m_parent->Instantiate<Gx::UiContainer>("IDC_CONTAINER_NOTE");
    m_container->SetBatchingEnabled(true);

    // Set-up Speed
    const auto speed = m_context->GetSpeed();
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

    // Load note templates
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
    const auto chart = m_context->GetChart();
    m_position  = 0;
    m_start     = 0;
    m_elapsed   = 0;
    m_reference = 0;
    m_bpm       = chart->GetMetadata().BPM;

    const auto events = chart->GetEvents(m_context->GetDifficulty());

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

    // TODO:
    // 1. Get Chart and Events
    // 2. Get Note Templates from "Require"
    // 3. Spawn (Both Circle and Square) Notes from the Templates to Note Container

    // TODO:
    // Add "viewport" attributes in Playing.json asset to determine play area (e.g Size <width?>, 480px)
    // X and Y coordinate should infer from the Note min X and min Y position
}

Gx::RenderStates ChartRenderer::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
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

    for (auto &ev : m_events)
    {
        const double position = ev->Position - m_position;
        if (ev.Accuracy != Accuracy::None)
            continue;

        if (!ev->IsPlayable())
        {
            if (position > 0)
                continue;

            if (ev->Channel == Chart::Channel::BPM)
            {
                const auto time = static_cast<Chart::TimeEvent*>(ev.Event);
                m_reference = m_position;
                m_start     = m_elapsed;
                m_bpm       = time->Value;
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

            ev.Accuracy = Accuracy::Cool;
            ev.Latency  = position;
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

    return states;
}

const GameContext &ChartRenderer::GetContext() const
{
    if (!m_context)
        throw Gx::Exception("ChartRenderer is not initialized yet");

    return *m_context;
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

