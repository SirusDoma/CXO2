#include <CXO2/Core/NoteGuideLine.hpp>
#include <CXO2/Core/Note.hpp>
#include <CXO2/Core/LongNote.hpp>
#include <CXO2/Core/ChartRenderer.hpp>

namespace Cx
{
    NoteGuideLine::NoteGuideLine(const Note& parent) :
        m_parent(&parent),
        m_guideLength(),
        m_delta()
    {
    }

    NoteGuideLine::NoteGuideLine(const LongNote& parent) :
        m_parent(&parent),
        m_guideLength(),
        m_delta()
    {
    }

    bool NoteGuideLine::IsVisible() const
    {
        return m_span.has_value() && m_span.value()[7].color.a > 0;
    }

    void NoteGuideLine::SetVisible(const bool visible)
    {
        if (!m_span.has_value() || IsVisible() == visible)
            return;

        for (auto& v : m_span.value())
            v.color.a = visible ? 255 : 0;
    }

    const Gx::VertexSpan& NoteGuideLine::GetVertices() const
    {
        return m_span.value();
    }

    void NoteGuideLine::SetVertices(Gx::VertexSpan&& vertices) noexcept
    {
        m_span = std::move(vertices);
    }

    void NoteGuideLine::Render(const ChartRenderer& renderer, const double delta)
    {
        if (!m_span.has_value() || !m_parent->IsVisible())
        {
            SetVisible(false);
            return;
        }

        auto& span = m_span.value();

        // Special thanks to MATERIALIZER
        // For reversing the original behavior of note guide-line on original O2Jam Client!

        // Determine base guide length and the speed multiplier factor length based on config
        float base = 0.f;
        float factor = 0.f;
        const float speed = renderer.GetSpeed(m_parent->GetChannel());
        switch (renderer.GetRenderSettings().Config.NoteGuideLength)
        {
            case 1:
                base   = 16.f;
                factor = 14.f;
                break;
            case 2:
                base   = 30.f;
                factor = 25.f;
                break;
            case 3:
                base   = 54.f;
                factor = 50.f;
                break;
            default:
                base   = 0.f;
                factor = 0.f;
                break;
        }

        // Calculate the length of guide. It is based on 1/16 length
        float guideLength = (((speed - 1.f) * factor + base) * renderer.GetCurrentBPM() * ((1.f / 16.f) / 10.f));
        if (guideLength > 120.f)
            guideLength = 120.f;

        // The line grow/shrunk at rate of 20ms per pixel when the current length is different from the desired length
        if (m_guideLength != guideLength)
        {
            m_delta += delta;
            if (m_delta >= (40.f / speed) / (renderer.GetCurrentBPM() / 60.f))
            {
                m_delta = 0;
                m_guideLength += (m_guideLength < guideLength) ? 1 : -1;
            }
        }
        else
            m_delta = 0;

        guideLength = m_guideLength;
        SetVisible(guideLength > 0);
        if (!IsVisible())
            return;

        const double distance = m_parent->GetRenderPosition() - renderer.GetRenderPosition();
        const auto shape = renderer.GetRenderSettings().Config.NoteShapeType;
        auto prefab = m_parent->GetPrefab(shape);

        float noteLength = 0.f;
        if (const auto ln = dynamic_cast<const LongNote*>(m_parent); ln)
        {
            prefab     = ln->GetEdgePrefab(shape);
            noteLength = ln->GetLength();
        }

        const auto transform = prefab ? prefab->GetTransform() : sf::Transform::Identity;
        const auto texCoords = prefab ? prefab->GetTexCoords() : sf::IntRect();
        const auto width     = prefab ? prefab->GetLocalBounds().size.x  : 0.f;
        const auto height    = prefab ? prefab->GetLocalBounds().size.y : 0.f;

        for (unsigned int i = 0; i < 4; i += 2)
        {
            // Head
            double target = distance;
            auto bounds   = sf::FloatRect({}, {0.f, guideLength - 1});

            // Tail
            if (i >= 2)
            {
                target = distance + noteLength;
                bounds   = sf::FloatRect({}, {0.f, -guideLength + 1});
            }

            const float pixels  = renderer.MapRenderPositionToPixels(m_parent->GetChannel(), target) + (height / 2.0f) - 0.1f;
            const auto position = transform.transformPoint(sf::Vector2f(1.f, pixels));

            span[i + 0].position = sf::Vector2f(position.x, position.y);
            span[i + 1].position = sf::Vector2f(position.x, position.y + bounds.size.y);

            span[i + 0].texCoords = sf::Vector2f(texCoords.position.x, texCoords.position.y);
            span[i + 1].texCoords = sf::Vector2f(texCoords.position.x, texCoords.position.y);

            span[i + 0].color = sf::Color(153, 153, 153, 230);
            span[i + 1].color = sf::Color::Black;
        }

        for (unsigned int i = 4; i < 8; i++)
        {
            span[i].position    = span[i - 4].position;
            span[i].position.x += width - 1; // let the grid occupy 1px of the note
            span[i].texCoords   = sf::Vector2f(texCoords.position.x, texCoords.position.y);
            span[i].texCoords   = sf::Vector2f(texCoords.position.x, texCoords.position.y);
            span[i].color       = span[i - 4].color;
        }
    }

    Gx::RenderStates NoteGuideLine::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        if (!m_span.has_value())
            return states;

        surface.Render(&m_span.value()[0], m_span->size(), sf::PrimitiveType::Triangles, states);
        return states;
    }
}
