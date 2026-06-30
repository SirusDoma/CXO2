#include <CXO2/Core/NoteContainer.hpp>
#include <CXO2/Core/ChartRenderer.hpp>

#include <CXO2/Core/Note.hpp>
#include <CXO2/Core/LongNote.hpp>

namespace Cx
{
    NoteContainer::NoteContainer() :
        m_renderer(),
        m_chart(),
        m_difficulty(),
        m_noteVertices(sf::PrimitiveType::Triangles),
        m_guideVertices(sf::PrimitiveType::Lines),
        m_notes(),
        m_measures(),
        m_shape(NoteShape::Square),
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

    Gx::VertexPool& NoteContainer::GetNoteVertexPool()
    {
        return m_noteVertices;
    }

    Gx::VertexPool& NoteContainer::GetGuideLineVertexPool()
    {
        return m_guideVertices;
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

    void NoteContainer::UpdateGeometry(const Chart::NoteEvent& ev, const sf::Time& delta) const
    {
        UpdateMeasures(delta);

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
        note->UpdateGeometry(*m_renderer, delta);
    }

    void NoteContainer::UpdateMeasures(const sf::Time& delta) const
    {
        if (m_tapCounter == 0)
        {
            double position = std::ceil(m_renderer->GetRenderPosition()) - 1.f;
            for (const auto measure : m_measures)
            {
                if (position < m_renderer->GetLastMeasurePosition())
                {
                    measure->SetVisible(true);
                    measure->SetRenderPosition(position);
                    measure->UpdateGeometry(*m_renderer, delta);
                }
                else
                    measure->SetVisible(false);

                position += m_chart->GetMeasureFraction(m_difficulty, position);
            }
        }
    }

    void NoteContainer::Update(const sf::Time& delta)
    {
        for (const auto updatable : GetRegisteredPrefabs())
            updatable->Update(delta);

        UpdatableContainer::Update(delta);
    }


    Gx::RenderStates NoteContainer::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        UpdateMeasures(states.Delta);
        for (std::size_t i = m_tapCounter; i < m_notes.size(); i++)
            m_notes[i]->SetVisible(false);

        for (std::size_t i = m_longCounter; i < m_longNotes.size(); i++)
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
            surface.Render(m_guideVertices, states);

            surface.SetView(currentView);
        }

        return RenderableContainer::Render(surface, states);
    }
}
