#include <OTwo/Core/NoteContainer.hpp>
#include <OTwo/Core/ChartRenderer.hpp>

NoteContainer::NoteContainer() :
    m_noteVertices(sf::PrimitiveType::Triangles),
    m_measureVertices(sf::PrimitiveType::Triangles),
    m_guideLineVertices(sf::PrimitiveType::Lines),
    m_notes(),
    m_shape(NoteShape::Square),
    m_lastMeasure()
{
}

void NoteContainer::Add(Note& note)
{
    m_notes[note.GetRenderPosition()].push_back(&note);
    if (const auto position = static_cast<unsigned int>(std::ceil(note.GetRenderPosition())); m_lastMeasure < position)
        m_lastMeasure = position;
}

Note *NoteContainer::GetNote(const Chart::Channel channel, const double position) const
{
    if (const auto it = m_notes.find(position); it != m_notes.end())
    {
        for (const auto note : it->second)
        {
            if (note->GetChannel() == channel)
                return note;
        }
    }

    return nullptr;
}

sf::FloatRect NoteContainer::GetViewport() const
{
    return m_viewport;
}

void NoteContainer::SetViewport(const sf::FloatRect &viewport)
{
    m_viewport = viewport;
}

sf::VertexArray& NoteContainer::GetNoteVertices()
{
    return m_noteVertices;
}

sf::VertexArray& NoteContainer::GetMeasureVertices()
{
    return m_measureVertices;
}

sf::VertexArray& NoteContainer::GetGuideLineVertices()
{
    return m_guideLineVertices;
}

const sf::Texture * NoteContainer::GetTexture(NoteShape shape)
{
    if (const auto it = m_textures.find(shape); it == m_textures.end())
        m_textures[shape] = nullptr;

    return m_textures[shape];
}

void NoteContainer::SetTexture(const NoteShape shape, const sf::Texture &texture)
{
    m_textures[shape] = &texture;
}

void NoteContainer::RegisterPrefab(Gx::Updatable &prefab)
{
    m_prefabs.insert(&prefab);
}

std::unordered_set<Gx::Updatable*> NoteContainer::GetRegisteredPrefabs()
{
    return m_prefabs;
}

unsigned int NoteContainer::GetLastMeasure() const
{
    return m_lastMeasure;
}

void NoteContainer::Update(const double delta)
{
    for (const auto updatable : GetRegisteredPrefabs())
        updatable->Update(delta);

    UpdatableContainer::Update(delta);
}

void NoteContainer::Render(const ChartRenderer &renderer, const double delta)
{
    m_shape = renderer.GetRenderSettings().Config->NoteShapeType;
    for (const auto& [_, group] : m_notes)
    {
        for (const auto& note : group)
            note->Render(renderer, delta);
    }
}

Gx::RenderStates NoteContainer::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    if (const auto it = m_textures.find(m_shape); it != m_textures.end())
    {
        // Use scissor test to mask the playable area
        const auto view = surface.GetView();
        if (m_viewport.width > 0 && m_viewport.height > 0)
        {
            auto viewport = surface.GetView();
            viewport.setScissor({
                { m_viewport.left  / view.getSize().x, 0.f },
                { m_viewport.width / view.getSize().x, m_viewport.height / view.getSize().y }
            });

            surface.SetView(viewport);
        }

        states.texture = it->second;
        surface.Render(m_measureVertices, states);
        surface.Render(m_noteVertices, states);

        states.texture = nullptr;
        surface.Render(m_guideLineVertices, states);

        surface.SetView(view);
    }

    return RenderableContainer::Render(surface, states);
}
