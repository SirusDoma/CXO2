#include <OTwo/Core/NoteFactory.hpp>
#include <OTwo/Core/ChartRenderer.hpp>

#include <OTwo/Core/Note.hpp>
#include <OTwo/Core/LongNote.hpp>
#include <OTwo/Core/NoteGuideLine.hpp>

NoteFactory::NoteFactory(Gx::ResourceManager& resources, const ChannelSet& instantiables) :
    NoteFactory(resources, resources, instantiables)
{
}

NoteFactory::NoteFactory(Gx::ResourceManager& instantiationResources, Gx::ResourceManager& prefabResources, const ChannelSet& instantiables) :
    m_resources(&instantiationResources),
    m_prefabResources(&prefabResources)
{
    const auto registerChannel = [this] (const Chart::Channel& channel) {
        if (channel != Chart::Channel::Measure && channel != Chart::Channel::BPM && channel != Chart::Channel::Background)
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

NoteContainer* NoteFactory::Generate(const ChartRenderer::RenderSettings& settings) const
{
    auto state = "STATE_PLAYING_" + std::to_string(m_channels.size()) + "K";
    const auto container = &m_resources->Create<NoteContainer>(state + "/IDC_NOTE_CONTAINER");
    auto tapNotePrefabs = PrefabMap();
    auto longNotePrefabs = PrefabMap();
    float x1 = 0, x2 = 0;
    for (auto channel : m_channels)
    {
        const int key = static_cast<std::uint16_t>(channel) - 1;
        tapNotePrefabs[channel] = {};
        longNotePrefabs[channel] = {};

        for (auto shape : { NoteShape::Square, NoteShape::Circle })
        {
            const int subKey = static_cast<std::uint8_t>(shape);

            const auto tap   = m_prefabResources->Find<Gx::Animation>(state + "/IDC_ANIMATION_NOTE_NORMAL" + std::to_string(key) + "_" + std::to_string(subKey));
            const auto hold  = m_prefabResources->Find<Gx::Animation>(state + "/IDC_ANIMATION_NOTE_LONG" + std::to_string(key) + "_" + std::to_string(subKey));

            container->RegisterPrefab(*tap);
            container->RegisterPrefab(*hold);

            tapNotePrefabs[channel][shape] = tap;
            longNotePrefabs[channel][shape] = hold;

            x1 = std::max(std::min(tap->GetPosition().x, x1), 0.f);
            x1 = std::max(std::min(hold->GetPosition().x, x1), 0.f);

            x2 = std::max(tap->GetPosition().x + tap->GetLocalBounds().size.x, x2);
            x2 = std::max(hold->GetPosition().x + hold->GetLocalBounds().size.x, x2);

            if (const auto texture = container->GetTexture(shape); !texture)
                container->SetTexture(shape, *tap->GetTexture());
            else if (texture != tap->GetTexture() || texture != hold->GetTexture())
                throw Gx::NotSupportedException("Note and Measure prefab must share same texture");
        }

        tapNotePrefabs[channel]= {
            { NoteShape::Square, m_prefabResources->Find<Gx::Animation>(state + "/IDC_ANIMATION_NOTE_NORMAL" + std::to_string(key) + "_1") },
            { NoteShape::Circle, m_prefabResources->Find<Gx::Animation>(state + "/IDC_ANIMATION_NOTE_NORMAL" + std::to_string(key) + "_2") },
        };

        longNotePrefabs[channel] = {
            { NoteShape::Square, m_prefabResources->Find<Gx::Animation>(state + "/IDC_ANIMATION_NOTE_LONG" + std::to_string(key) + "_1") },
            { NoteShape::Circle, m_prefabResources->Find<Gx::Animation>(state + "/IDC_ANIMATION_NOTE_LONG" + std::to_string(key) + "_2") },
        };
    }

    // Viewport is calculated based on Min X and Max X notes on all channels
    container->SetViewport({ { x1, 0 }, { x2, static_cast<float>(settings.Viewport) }});

    // TODO: Apply Arrangement Modifiers

    // Initializes vertices and reserve the array to prevent vertex address from shifting
    auto& vertices = container->GetNoteVertices();
    auto& guideLineVertices = container->GetGuideLineVertices();

    // Resize the vertices to buffer size
    constexpr unsigned int bufferSize = 192 * 20 * 2;
    vertices.resize((bufferSize * 6 * 3) + (20 * 6));
    guideLineVertices.resize(bufferSize * 2 * 4);

    // TODO: Better vertices index tracking and allocation
    unsigned int vi = 0;
    unsigned int vg = 0;

    // Prepare  measure prefabs
    auto measurePrefabs = PrefabMap();
    measurePrefabs[Chart::Channel::Background] =
    {
        { NoteShape::Square, m_prefabResources->Find<Gx::Sprite>(state + "/IDC_IMAGE_NOTE_MEASURE1") },
        { NoteShape::Circle, m_prefabResources->Find<Gx::Sprite>(state + "/IDC_IMAGE_NOTE_MEASURE2") }
    };

    container->RegisterPrefab(*measurePrefabs[Chart::Channel::Background][NoteShape::Square]);
    container->RegisterPrefab(*measurePrefabs[Chart::Channel::Background][NoteShape::Circle]);

    // Set-up temp vertices
    auto vx = std::array<sf::Vertex*, 6>();
    auto vz = std::array<sf::Vertex*, 8>();

    // Configure measure vertices
    for (unsigned int m = 1; m <= 20; m++)
    {
        auto& measure = m_resources->Create<Measure>("IDC_MEASURE_" + std::to_string(m), m, Chart::Channel::Background);

        for (std::size_t v = 0; v < vx.size(); v++)
            vx[v] = &vertices[vi + v];

        vi += 6;
        measure.SetVertices(vx);
        measure.SetPrefabs(measurePrefabs);


        container->AddMeasure(measure);
    }

    // Configure note vertices
    for (unsigned int i = 0; i < bufferSize; i++)
    {
        if (i < bufferSize / 2)
        {
            auto& note = m_resources->Create<Note>(state + "/IDC_TAP_NOTE_" + std::to_string(i), 0, Chart::Channel::Note4);
            for (std::size_t v = 0; v < vx.size(); v++)
                vx[v] = &vertices[vi + v];

            vi += 6;
            note.SetVertices(vx);
            note.SetPrefabs(tapNotePrefabs);

            for (std::size_t v = 0; v < vz.size(); v++)
                vz[v] = &guideLineVertices[vg + v];

            vg += 8;
            note.GetGuideLine()->SetVertices(vz);

            container->AddNote(note);
        }
        else
        {
            auto& longNote = m_resources->Create<LongNote>(state + "/IDC_LONG_NOTE_" + std::to_string(i), 0, 1, Chart::Channel::Note4);
            for (std::size_t v = 0; v < vx.size(); v++)
                vx[v] = &vertices[vi + v];

            vi += 6;
            longNote.SetVertices(vx);
            longNote.SetPrefabs(longNotePrefabs);

            for (std::size_t v = 0; v < vx.size(); v++)
                vx[v] = &vertices[vi + v];

            vi += 6;
            longNote.SetHeadVertices(vx);

            for (std::size_t v = 0; v < vx.size(); v++)
                vx[v] = &vertices[vi + v];

            vi += 6;
            longNote.SetTailVertices(vx);

            for (std::size_t v = 0; v < vz.size(); v++)
                vz[v] = &guideLineVertices[vg + v];

            vg += 8;
            longNote.GetGuideLine()->SetVertices(vz);

            longNote.SetEdgePrefabs(tapNotePrefabs);
            container->AddNote(longNote);
        }
    }

    // Reduce the vertices number to actual usage
    vertices.resize(vi);
    guideLineVertices.resize(vg);

    return container;
}
