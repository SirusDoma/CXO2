#include <CXO2/Core/NoteFactory.hpp>
#include <CXO2/Core/ChartRenderer.hpp>
#include <CXO2/Core/Note.hpp>
#include <CXO2/Core/LongNote.hpp>
#include <CXO2/Core/NoteGuideLine.hpp>

#include <CXO2/Constants/Identifiers/Game.hpp>

#include <fmt/format.h>

namespace Cx
{
    using namespace Constants::Identifiers;

    // TODO: Pass State instead
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

    NoteContainer& NoteFactory::Generate(const ChartRenderer::RenderSettings& settings) const
    {
        auto state           = fmt::format("STATE_PLAYING_{}K", m_channels.size());
        auto& container      = m_resources->Create<NoteContainer>(fmt::format("{}/{}", state, Resource::Game::Renderer::IDC_NOTE_CONTAINER));
        auto tapNotePrefabs  = PrefabMap();
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

                const auto tap  = dynamic_cast<Gx::Animation*>(m_prefabResources->Find<Gx::Node>(fmt::format("{}/{}", state, Resource::Game::Renderer::IDC_ANIMATION_NOTE_NORMAL(key, subKey))));
                const auto hold = dynamic_cast<Gx::Animation*>(m_prefabResources->Find<Gx::Node>(fmt::format("{}/{}", state, Resource::Game::Renderer::IDC_ANIMATION_NOTE_LONG(key, subKey))));

                container.RegisterPrefab(*tap);
                container.RegisterPrefab(*hold);

                tapNotePrefabs[channel][shape] = tap;
                longNotePrefabs[channel][shape] = hold;

                x1 = std::max(std::min(tap->GetPosition().x, x1), 0.f);
                x1 = std::max(std::min(hold->GetPosition().x, x1), 0.f);

                x2 = std::max(tap->GetPosition().x + tap->GetLocalBounds().size.x, x2);
                x2 = std::max(hold->GetPosition().x + hold->GetLocalBounds().size.x, x2);

                if (const auto texture = container.GetTexture(shape); !texture)
                    container.SetTexture(shape, *tap->GetTexture());
                else if (texture != tap->GetTexture() || texture != hold->GetTexture())
                    throw Gx::NotSupportedException("Note and Measure prefab must share same texture");
            }
        }

        // Viewport is calculated based on Min X and Max X notes on all channels
        container.SetViewport({ { x1, 0 }, { x2, static_cast<float>(settings.Viewport) }});

        // TODO: Apply Arrangement Modifiers

        // Initializes vertices by using vertex pool
        constexpr unsigned int bufferSize = 192 * 20 * 2;
        auto& vertices = container.GetNoteVertexPool();
        auto& guideLineVertices = container.GetGuideLineVertexPool();

        // Prepare  measure prefabs
        auto measurePrefabs = PrefabMap();
        measurePrefabs[Chart::Channel::Background] =
        {
            { NoteShape::Square, dynamic_cast<Gx::Sprite*>(m_prefabResources->Find<Gx::Node>(fmt::format("{}/{}", state, Resource::Game::Renderer::IDC_IMAGE_NOTE_MEASURE1))) },
            { NoteShape::Circle, dynamic_cast<Gx::Sprite*>(m_prefabResources->Find<Gx::Node>(fmt::format("{}/{}", state, Resource::Game::Renderer::IDC_IMAGE_NOTE_MEASURE2))) }
        };

        container.RegisterPrefab(*measurePrefabs[Chart::Channel::Background][NoteShape::Square]);
        container.RegisterPrefab(*measurePrefabs[Chart::Channel::Background][NoteShape::Circle]);

        // Configure measure vertices
        for (unsigned int m = 1; m <= 20; m++)
        {
            auto& measure = m_resources->Create<Measure>(fmt::format("{}/{}", state, Resource::Game::Renderer::IDC_MEASURE(m)), m, Chart::Channel::Background);

            measure.SetVertices(vertices.Rent(6));
            measure.SetPrefabs(measurePrefabs);

            container.AddMeasure(measure);
        }

        // Configure note vertices
        for (unsigned int i = 0; i < bufferSize; i++)
        {
            if (i < bufferSize / 2)
            {
                auto& note = m_resources->Create<Note>(fmt::format("{}/{}", state, Resource::Game::Renderer::IDC_TAP_NOTE(i)), 0, Chart::Channel::Note4);

                note.SetVertices(vertices.Rent(6));
                note.SetPrefabs(tapNotePrefabs);

                note.GetGuideLine()->SetVertices(guideLineVertices.Rent(8));

                container.AddNote(note);
            }
            else
            {
                auto& longNote = m_resources->Create<LongNote>(fmt::format("{}/{}", state, Resource::Game::Renderer::IDC_LONG_NOTE(i)), 0, 1, Chart::Channel::Note4);

                longNote.SetVertices(vertices.Rent(6));
                longNote.SetPrefabs(longNotePrefabs);

                longNote.SetHeadVertices(vertices.Rent(6));
                longNote.SetTailVertices(vertices.Rent(6));

                longNote.GetGuideLine()->SetVertices(guideLineVertices.Rent(8));

                longNote.SetEdgePrefabs(tapNotePrefabs);
                container.AddNote(longNote);
            }
        }

        return container;
    }
}
