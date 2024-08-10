#include <OTwo/Chart/NoteFactory.hpp>
#include <OTwo/Chart/ChartRenderer.hpp>

#include <OTwo/Chart/Note.hpp>
#include <OTwo/Chart/LongNote.hpp>
#include <OTwo/Chart/NoteGuideLine.hpp>

NoteFactory::NoteFactory(Gx::ResourceManager &resources, const ChannelSet &instantiables) :
    NoteFactory(resources, resources, instantiables)
{
}

NoteFactory::NoteFactory(Gx::ResourceManager &instantiationResources, Gx::ResourceManager &prefabResources, const ChannelSet &instantiables) :
    m_resources(&instantiationResources),
    m_prefabResources(&prefabResources)
{
    const auto registerChannel = [this] (const Chart::Channel &channel) {
        if (channel != Chart::Channel::Measurement && channel != Chart::Channel::BPM && channel != Chart::Channel::Background)
            m_channels.insert(channel);
    };

    if (!instantiables.empty())
    {
        for (auto channel : instantiables)
            registerChannel(channel);
    }
    else
    {
        for (auto channel : Chart::PlayableChannels)
            registerChannel(channel);
    }
}

NoteContainer *NoteFactory::Generate(const Chart &chart, const ChartRenderer::RenderSettings &settings) const
{
    const auto container = &m_resources->Create<NoteContainer>("STATE_PLAYING/IDC_NOTE_CONTAINER");
    auto tapNotePrefabs = PrefabMap();
    auto longNotePrefabs = PrefabMap();
    float x1 = 0, x2 = 0;
    for (auto channel : m_channels)
    {
        const int key = static_cast<Gx::Uint16>(channel) - 1;
        tapNotePrefabs[channel] = {};
        longNotePrefabs[channel] = {};

        for (auto shape : { NoteShape::Square, NoteShape::Circle })
        {
            const int subKey = static_cast<Gx::Uint8>(shape);

            const auto tap   = m_prefabResources->Find<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_NORMAL" + std::to_string(key) + "_" + std::to_string(subKey));
            const auto hold  = m_prefabResources->Find<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_LONG" + std::to_string(key) + "_" + std::to_string(subKey));

            container->RegisterPrefab(*tap);
            container->RegisterPrefab(*hold);

            tapNotePrefabs[channel][shape] = tap;
            longNotePrefabs[channel][shape] = hold;

            x1 = std::max(std::min(tap->GetPosition().x, x1), 0.f);
            x1 = std::max(std::min(hold->GetPosition().x, x1), 0.f);

            x2 = std::max(tap->GetPosition().x + tap->GetLocalBounds().width, x2);
            x2 = std::max(hold->GetPosition().x + hold->GetLocalBounds().width, x2);

            if (const auto texture = container->GetTexture(shape); !texture)
                container->SetTexture(shape, *tap->GetTexture());
            else if (texture != tap->GetTexture() || texture != hold->GetTexture())
                throw Gx::NotSupportedException("Note and Measure prefab must share same texture.");
        }

        tapNotePrefabs[channel]= {
            { NoteShape::Square, m_prefabResources->Find<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_NORMAL" + std::to_string(key) + "_1") },
            { NoteShape::Circle, m_prefabResources->Find<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_NORMAL" + std::to_string(key) + "_2") },
        };

        longNotePrefabs[channel] = {
            { NoteShape::Square, m_prefabResources->Find<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_LONG" + std::to_string(key) + "_1") },
            { NoteShape::Circle, m_prefabResources->Find<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_LONG" + std::to_string(key) + "_2") },
        };
    }

    // Viewport is calculated based on Min X and Max X notes on all channels
    container->SetViewport({ { x1, 0 }, { x2, static_cast<float>(settings.Viewport) }});

    // TODO: Apply Arrangement Modifiers

    unsigned int max = 0;
    const auto events = chart.GetEvents(settings.Difficulty);

    // Initializes vertices and reserve the array to prevent vertex address from shifting
    auto& vertices = container->GetNoteVertices();
    auto& guideLineVertices = container->GetGuideLineVertices();

    // Resize to worst case scenario (which is LN)
    vertices.resize(events.size() * 6 * 3); // LN = 3 objects (head, tail, and body)
    guideLineVertices.resize(events.size() * 2 * 4); // 4 objects (head (left + right), tail (left + right))

    // TODO: Better vertices index tracking and allocation
    unsigned int vi = 0;
    unsigned int vg = 0;
    for (int i = 0; i < events.size(); i++)
    {
        max = max < (events[i] ? std::ceil(events[i]->Position) : 0) ? std::ceil(events[i]->Position) : max;
        if (!events[i] || events[i]->Channel == Chart::Channel::Measurement || events[i]->Channel == Chart::Channel::BPM || events[i]->Channel == Chart::Channel::Background)
            continue;

        const auto ev = static_cast<Chart::NoteEvent&>(*events[i]);
        if (ev.Type == Chart::NoteType::Tap)
        {
            auto& note = m_resources->Create<Note>("STATE_PLAYING/IDC_TAP_NOTE_" + std::to_string(i), ev.Position, ev.Channel);
            auto vx = std::array<sf::Vertex*, 6>();

            for (int v = 0; v < vx.size(); v++)
                vx[v] = &vertices[vi + v];

            vi += 6;
            note.SetVertices(vx);
            for (const auto shape : { NoteShape::Square, NoteShape::Circle })
                note.SetPrefab(shape, *tapNotePrefabs[ev.Channel][shape]);

            auto vz = std::array<sf::Vertex*, 8>();
            for (int v = 0; v < vz.size(); v++)
                vz[v] = &guideLineVertices[vg + v];

            vg += 8;
            note.GetGuideLine()->SetVertices(vz);

            container->Add(note);
        }
        else if (ev.Type == Chart::NoteType::Hold)
        {
            auto& longNote = m_resources->Create<LongNote>("STATE_PLAYING/IDC_LONG_NOTE_" + std::to_string(i), ev.Position, ev.Length, ev.Channel);
            auto vx = std::array<sf::Vertex*, 6>();

            for (int v = 0; v < vx.size(); v++)
                vx[v] = &vertices[vi + v];

            vi += 6;
            longNote.SetVertices(vx);
            for (const auto shape : { NoteShape::Square, NoteShape::Circle })
                longNote.SetPrefab(shape, *longNotePrefabs[ev.Channel][shape]);

            vx = std::array<sf::Vertex*, 6>();
            for (int v = 0; v < vx.size(); v++)
                vx[v] = &vertices[vi + v];

            vi += 6;
            longNote.SetHeadVertices(vx);

            vx = std::array<sf::Vertex*, 6>();
            for (int v = 0; v < vx.size(); v++)
                vx[v] = &vertices[vi + v];

            vi += 6;
            longNote.SetTailVertices(vx);

            auto vz = std::array<sf::Vertex*, 8>();
            for (int v = 0; v < vz.size(); v++)
                vz[v] = &guideLineVertices[vg + v];

            vg += 8;
            longNote.GetGuideLine()->SetVertices(vz);

            for (const auto shape : { NoteShape::Square, NoteShape::Circle })
                longNote.SetEdgePrefab(shape, *tapNotePrefabs[ev.Channel][shape]);

            container->Add(longNote);
        }
    }

    // Reduce the vertices number to actual usage
    vertices.resize(vi);
    guideLineVertices.resize(vg);

    std::unordered_map<NoteShape, Gx::Sprite*> measurePrefab = {
        { NoteShape::Square, m_prefabResources->Find<Gx::Sprite>("STATE_PLAYING/IDC_IMAGE_NOTE_MEASURE1") },
        { NoteShape::Circle, m_prefabResources->Find<Gx::Sprite>("STATE_PLAYING/IDC_IMAGE_NOTE_MEASURE2") }
    };
    container->RegisterPrefab(*measurePrefab[NoteShape::Square]);
    container->RegisterPrefab(*measurePrefab[NoteShape::Circle]);

    vi = 0;
    max += 1;

    auto& measureVertices = container->GetMeasureVertices();
    measureVertices.resize(max * 6);

    for (int m = 0; m < max; m++)
    {
        auto& measure = m_resources->Create<Measure>("IDC_MEASURE_" + std::to_string(m), m, Chart::Channel::Background);
        auto vx = std::array<sf::Vertex*, 6>();

        for (int v = 0; v < vx.size(); v++)
            vx[v] = &measureVertices[vi + v];

        vi += 6;
        measure.SetVertices(vx);
        for (const auto shape : { NoteShape::Square, NoteShape::Circle })
            measure.SetPrefab(shape, *measurePrefab[shape]);

        container->Add(measure);
    }

    return container;
}
