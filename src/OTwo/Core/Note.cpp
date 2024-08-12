#include <OTwo/Core/Note.hpp>
#include <OTwo/Core/ChartRenderer.hpp>

Note::Note(const Chart::NoteEvent &ev) :
    Note(ev.Position, ev.Channel)
{
}

Note::Note(const double position, const Chart::Channel channel) :
    m_vertices(),
    m_position(position),
    m_channel(channel),
    m_line(*this),
    m_hit(false)
{
    for (int i = 0; i < m_vertices.size(); i++)
        m_vertices[i] = nullptr;
}

double Note::GetRenderPosition() const
{
    return m_position;
}

Chart::Channel Note::GetChannel() const
{
    return m_channel;
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
    if (const auto it = m_prefabs.find(shape); it != m_prefabs.end())
        return it->second;

    return nullptr;
}

void Note::SetPrefab(const NoteShape shape, Gx::Sprite &prefab)
{
    m_prefabs[shape] = &prefab;
}

bool Note::IsVisible() const
{
    return m_vertices[0] && m_vertices[0]->color.a > 0;
}

void Note::SetVisible(const bool visible)
{
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

void Note::Hit()
{
    m_hit = true;
}

void Note::Render(const ChartRenderer &renderer, const double delta)
{
    if (!m_vertices[0])
        return;

    const double latency = m_position - renderer.GetRenderPosition();
    if (m_hit || latency > 5.f || latency < -0.5f)
    {
        if (m_hit)
        {
            SetVisible(false);
            GetGuideLine()->Render(renderer, delta);
        }

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

void Note::UpdatePositions(const VerticesPtr& vertices, const sf::Vector2f &position, const sf::FloatRect &bounds)
{
    vertices[0]->position = sf::Vector2f(position.x, position.y);
    vertices[1]->position = sf::Vector2f(position.x + bounds.width, position.y);
    vertices[2]->position = sf::Vector2f(position.x + bounds.width, position.y + bounds.height);
    vertices[3]->position = sf::Vector2f(position.x, position.y);
    vertices[4]->position = sf::Vector2f(position.x + bounds.width, position.y + bounds.height);
    vertices[5]->position = sf::Vector2f(position.x, position.y + bounds.height);
}

void Note::UpdateTexCoords(const VerticesPtr& vertices, const sf::IntRect &texcoords)
{
    const float left     = static_cast<float>(texcoords.left);
    const float right    = left + static_cast<float>(texcoords.width);
    const float top      = static_cast<float>(texcoords.top);
    const float bottom   = top + static_cast<float>(texcoords.height);

    vertices[0]->texCoords = sf::Vector2f(left, top);
    vertices[1]->texCoords = sf::Vector2f(right, top);
    vertices[2]->texCoords = sf::Vector2f(right, bottom);
    vertices[3]->texCoords = sf::Vector2f(left, top);
    vertices[4]->texCoords = sf::Vector2f(right, bottom);
    vertices[5]->texCoords = sf::Vector2f(left, bottom);
}

Gx::RenderStates Note::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    for (const auto v : m_vertices)
    {
        if (!v)
            return states;
    }

    surface.Render(m_vertices[0], m_vertices.size(), sf::PrimitiveType::Triangles, states);
    return states;
}
