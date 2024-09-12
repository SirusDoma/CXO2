#include <OTwo/Core/Note.hpp>
#include <OTwo/Core/ChartRenderer.hpp>

Note::Note(const Chart::NoteEvent& ev) :
    Note(ev.Position, ev.Channel)
{
}

Note::Note(const double position, const Chart::Channel channel) :
    m_vertices(),
    m_position(position),
    m_channel(channel),
    m_line(*this)
{
    for (int i = 0; i < m_vertices.size(); i++)
        m_vertices[i] = nullptr;
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

void Note::SetChannel(Chart::Channel channel)
{
    m_channel = channel;
}

NoteGuideLine* Note::GetGuideLine()
{
    return &m_line;
}

const std::array<sf::Vertex*, 6>& Note::GetVertices() const
{
    return m_vertices;
}

void Note::SetVertices(const std::array<sf::Vertex*, 6> &vertices)
{
    m_vertices = vertices;
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
    return m_vertices[0] && m_vertices[5]->color.a > 0;
}

void Note::SetVisible(const bool visible)
{
    if (IsVisible() == visible)
        return;

    GetGuideLine()->SetVisible(visible);
    for (const auto v : m_vertices)
    {
        if (!v)
            break;

        if (!visible)
        {
            v->position  = sf::Vector2f();
            v->texCoords = sf::Vector2f();
            v->color     = sf::Color::Transparent;
        }
        else
            v->color = sf::Color::White;
    }
}

void Note::Render(const ChartRenderer& renderer, const double delta)
{
    if (!m_vertices[0])
        return;

    const double latency = m_position - renderer.GetRenderPosition();
    if (latency > 5.f || latency < -0.5f)
    {
        SetVisible(false);
        GetGuideLine()->Render(renderer, delta);

        return;
    }

    const Gx::Sprite* sprite = GetPrefab(renderer.GetRenderSettings().Config->NoteShapeType);
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

    UpdatePositions(m_vertices, position, bounds);
    UpdateTexCoords(m_vertices, sprite->GetTexCoords());
}

void Note::UpdatePositions(const VerticesPtr& vertices, const sf::Vector2f& position, const sf::FloatRect& bounds)
{
    vertices[0]->position = sf::Vector2f(position.x, position.y);
    vertices[1]->position = sf::Vector2f(position.x + bounds.size.x, position.y);
    vertices[2]->position = sf::Vector2f(position.x + bounds.size.x, position.y + bounds.size.y);
    vertices[3]->position = sf::Vector2f(position.x, position.y);
    vertices[4]->position = sf::Vector2f(position.x + bounds.size.x, position.y + bounds.size.y);
    vertices[5]->position = sf::Vector2f(position.x, position.y + bounds.size.y);
}

void Note::UpdateTexCoords(const VerticesPtr& vertices, const sf::IntRect& texcoords)
{
    const float left     = static_cast<float>(texcoords.position.x);
    const float right    = left + static_cast<float>(texcoords.size.x);
    const float top      = static_cast<float>(texcoords.position.y);
    const float bottom   = top + static_cast<float>(texcoords.size.y);

    vertices[0]->texCoords = sf::Vector2f(left, top);
    vertices[1]->texCoords = sf::Vector2f(right, top);
    vertices[2]->texCoords = sf::Vector2f(right, bottom);
    vertices[3]->texCoords = sf::Vector2f(left, top);
    vertices[4]->texCoords = sf::Vector2f(right, bottom);
    vertices[5]->texCoords = sf::Vector2f(left, bottom);
}

Gx::RenderStates Note::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
{
    for (const auto v : m_vertices)
    {
        if (!v)
            return states;
    }

    surface.Render(m_vertices[0], m_vertices.size(), sf::PrimitiveType::Triangles, states);
    return states;
}
