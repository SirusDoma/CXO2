#include <OTwo/Core/NoteContainer.hpp>
#include <OTwo/Core/ChartRenderer.hpp>

#include <OTwo/Core/Note.hpp>
#include <OTwo/Core/LongNote.hpp>

NoteContainer::NoteContainer() :
    m_renderer(),
    m_chart(),
    m_difficulty(),
    m_noteVertices(sf::PrimitiveType::Triangles),
    m_measureVertices(sf::PrimitiveType::Triangles),
    m_guideLineVertices(sf::PrimitiveType::Lines),
    m_notes(),
    m_measures(),
    m_shape(NoteShape::Square),
    m_lastMeasure(),
    m_tapCounter(0),
    m_longCounter(0)
{
}

void NoteContainer::Initialize(const ChartRenderer& renderer, const Chart& chart, const Difficulty difficulty)
{
    m_renderer = &renderer;
    m_chart = &chart;
    m_difficulty = difficulty;
}

void NoteContainer::AddNote(Note& note)
{
    if (const auto ln = dynamic_cast<LongNote*>(&note); ln)
        m_longNotes.push_back(ln);
    else
        m_notes.push_back(&note);

    if (const auto position = static_cast<unsigned int>(std::ceil(note.GetRenderPosition())); m_lastMeasure < position)
        m_lastMeasure = position;
}

void NoteContainer::AddMeasure(Measure& measure)
{
    m_measures.push_back(&measure);
}

Note* NoteContainer::GetNote(const Chart::Channel channel, const Chart::NoteType type, const double position) const
{
    for (const auto note : m_notes)
    {
        if (note->GetRenderPosition() == position && note->GetChannel() == channel)
            return note;
    }

    return nullptr;
}

sf::FloatRect NoteContainer::GetViewport() const
{
    return m_viewport;
}

void NoteContainer::SetViewport(const sf::FloatRect& viewport)
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

const sf::Texture* NoteContainer::GetTexture(NoteShape shape)
{
    if (const auto it = m_textures.find(shape); it == m_textures.end())
        m_textures[shape] = nullptr;

    return m_textures[shape];
}

void NoteContainer::SetTexture(const NoteShape shape, const sf::Texture& texture)
{
    m_textures[shape] = &texture;
}

void NoteContainer::RegisterPrefab(Gx::Updatable& prefab)
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

void NoteContainer::Render(const Chart::NoteEvent& ev, const double delta) const
{
    RenderMeasures(delta);

    Note* note = nullptr;
    if (ev.Type == Chart::NoteType::Hold)
    {
        if (m_longCounter >= m_longNotes.size())
            return;

        note = m_longNotes[m_longCounter++];
        static_cast<LongNote*>(note)->SetLength(ev.Length);
    }
    else
    {
        if (m_tapCounter >= m_notes.size())
            return;

        note = m_notes[m_tapCounter++];
    }

    note->SetRenderPosition(ev.Position);
    note->SetChannel(ev.Channel);
    note->SetVisible(true);
    note->Render(*m_renderer, delta);
}

void NoteContainer::RenderMeasures(double delta) const
{
    if (m_tapCounter == 0)
    {
        double position = std::ceil(m_renderer->GetRenderPosition()) - 1.f;
        for (const auto measure : m_measures)
        {
            measure->SetRenderPosition(position);
            measure->Render(*m_renderer, delta);

            position += m_chart->GetMeasureFraction(m_difficulty, position);
        }
    }
}

void NoteContainer::Update(const double delta)
{
    for (const auto updatable : GetRegisteredPrefabs())
        updatable->Update(delta);

    UpdatableContainer::Update(delta);
}


Gx::RenderStates NoteContainer::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
{
    RenderMeasures(states.Delta);
    for (int i = m_tapCounter; i < m_notes.size(); i++)
        m_notes[i]->SetVisible(false);

    for (int i = m_longCounter; i < m_longNotes.size(); i++)
        m_longNotes[i]->SetVisible(false);

    m_tapCounter = 0;
    m_longCounter = 0;
    if (const auto it = m_textures.find(m_shape); it != m_textures.end())
    {
        // Use scissor test to mask the playable area
        const auto currentView = surface.GetView();
        if (m_viewport.size.x > 0 && m_viewport.size.y > 0)
        {
            auto scissorView = surface.GetView();
            auto area  = sf::FloatRect({
                { scissorView.getViewport().position.x + (m_viewport.position.x / scissorView.getSize().x), 0.f },
                { m_viewport.size.x / scissorView.getSize().x, m_viewport.size.y / scissorView.getSize().y }
            });

            if (area.position.x < -1.f || area.position.x > 1.f)
                area.position.x = scissorView.getViewport().position.x;

            scissorView.setScissor(area);
            surface.SetView(scissorView);
        }

        states.texture = it->second;
        surface.Render(m_noteVertices, states);

        states.texture = nullptr;
        surface.Render(m_guideLineVertices, states);

        surface.SetView(currentView);
    }

    return RenderableContainer::Render(surface, states);
}
