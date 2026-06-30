#include <CXO2/Core/Note.hpp>
#include <CXO2/Core/ChartRenderer.hpp>

namespace Cx
{
    Note::Note(const Chart::NoteEvent& ev) :
        Note(ev.Position, ev.Channel)
    {
    }

    Note::Note(const double position, const Chart::Channel channel) :
        m_position(position),
        m_channel(channel),
        m_line(*this)
    {
    }

    double Note::GetRenderPosition() const
    {
        return m_position;
    }

    void Note::SetRenderPosition(const double position)
    {
        m_position = position;
    }

    Chart::Channel Note::GetChannel() const
    {
        return m_channel;
    }

    void Note::SetChannel(const Chart::Channel channel)
    {
        m_channel = channel;
    }

    NoteGuideLine* Note::GetGuideLine()
    {
        return &m_line;
    }

    const Gx::VertexSpan& Note::GetVertices() const
    {
        return m_span.value();
    }

    Gx::VertexSpan& Note::GetVertices()
    {
        return m_span.value();
    }

    void Note::SetVertices(Gx::VertexSpan&& vertices) noexcept
    {
        m_span = std::move(vertices);
    }

    const Gx::Sprite* Note::GetPrefab(const NoteShape shape) const
    {
        if (const auto it = m_prefabs.find(m_channel); it != m_prefabs.end())
        {
            if (const auto it2 = it->second.find(shape); it2 != it->second.end())
                return it2->second;
        }

        return nullptr;
    }

    void Note::SetPrefabs(const PrefabMap& prefabs)
    {
        m_prefabs = prefabs;
    }

    bool Note::IsVisible() const
    {
        return m_span.has_value() && !m_span->empty() && m_span.value()[5].color.a > 0;
    }

    void Note::SetVisible(const bool visible)
    {
        if (!m_span.has_value() || m_span->empty() || IsVisible() == visible)
            return;

        GetGuideLine()->SetVisible(visible);
        for (auto& v : m_span.value())
        {
            if (!visible)
            {
                v.position  = sf::Vector2f();
                v.texCoords = sf::Vector2f();
                v.color     = sf::Color::Transparent;
            }
            else
                v.color = sf::Color::White;
        }
    }

    void Note::UpdateGeometry(const ChartRenderer& renderer, const double delta)
    {
        if (!m_span.has_value() || m_span->empty())
            return;

        auto& span = m_span.value();

        const double latency = m_position - renderer.GetRenderPosition();
        if (latency > 5.f || latency < -0.5f)
        {
            SetVisible(false);
            GetGuideLine()->Render(renderer, delta);

            return;
        }

        const Gx::Sprite* sprite = GetPrefab(renderer.GetRenderSettings().Config.NoteShapeType);
        if (!sprite)
        {
            SetVisible(false);
            GetGuideLine()->Render(renderer, delta);

            return;
        }

        SetVisible(true);
        GetGuideLine()->Render(renderer, delta);

        const auto transform = sprite->GetTransform();
        const auto position  = transform.transformPoint(sf::Vector2f(0, renderer.MapRenderPositionToPixels(GetChannel(), latency)));
        const auto bounds    = transform.transformRect(sprite->GetLocalBounds());

        UpdatePositions(span, position, bounds);
        UpdateTexCoords(span, sprite->GetTexCoords());
    }

    void Note::UpdatePositions(Gx::VertexSpan& span, const sf::Vector2f& position, const sf::FloatRect& bounds)
    {
        span[0].position = sf::Vector2f(position.x, position.y);
        span[1].position = sf::Vector2f(position.x + bounds.size.x, position.y);
        span[2].position = sf::Vector2f(position.x + bounds.size.x, position.y + bounds.size.y);
        span[3].position = sf::Vector2f(position.x, position.y);
        span[4].position = sf::Vector2f(position.x + bounds.size.x, position.y + bounds.size.y);
        span[5].position = sf::Vector2f(position.x, position.y + bounds.size.y);
    }

    void Note::UpdateTexCoords(Gx::VertexSpan& span, const sf::IntRect& texcoords)
    {
        const float left     = static_cast<float>(texcoords.position.x);
        const float right    = left + static_cast<float>(texcoords.size.x);
        const float top      = static_cast<float>(texcoords.position.y);
        const float bottom   = top + static_cast<float>(texcoords.size.y);

        span[0].texCoords = sf::Vector2f(left, top);
        span[1].texCoords = sf::Vector2f(right, top);
        span[2].texCoords = sf::Vector2f(right, bottom);
        span[3].texCoords = sf::Vector2f(left, top);
        span[4].texCoords = sf::Vector2f(right, bottom);
        span[5].texCoords = sf::Vector2f(left, bottom);
    }

    Gx::RenderStates Note::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        if (!m_span.has_value() || m_span->empty())
            return states;

        surface.Render(m_span->data(), m_span->size(), sf::PrimitiveType::Triangles, states);
        return states;
    }
}
