#include <OTwo/Core/LongNote.hpp>
#include <OTwo/Core/ChartRenderer.hpp>

LongNote::LongNote(const Chart::NoteEvent &ev) :
    LongNote(ev.Position, ev.Length, ev.Channel)
{
}

LongNote::LongNote(const double position, double length, const Chart::Channel channel) :
    Note(position, channel),
    m_headVertices(),
    m_tailVertices(),
    m_length(length)
{
}

bool LongNote::IsVisible() const
{
    return Note::IsVisible();
}

void LongNote::SetVisible(const bool visible)
{
    for (const auto v : GetVertices())
    {
        if (!v)
            break;

        v->color = visible ? sf::Color::White : sf::Color::Transparent;
    }

    for (const auto v : m_headVertices)
    {
        if (!v)
            break;

        v->color = visible ? sf::Color::White : sf::Color::Transparent;
    }

    for (const auto v : m_tailVertices)
    {
        if (!v)
            break;

        v->color = visible ? sf::Color::White : sf::Color::Transparent;
    }
}

double LongNote::GetLength() const
{
    return m_length;
}

void LongNote::SetLength(const double length)
{
    m_length = length;
}

Note::VerticesPtr LongNote::GetHeadVertices() const
{
    return m_headVertices;
}

void LongNote::SetHeadVertices(const std::array<sf::Vertex*, 6>& vertices)
{
    m_headVertices = vertices;
}

Note::VerticesPtr LongNote::GetTailVertices() const
{
    return m_tailVertices;
}

void LongNote::SetTailVertices(const std::array<sf::Vertex*, 6>& vertices)
{
    m_tailVertices = vertices;
}

const Gx::Sprite* LongNote::GetEdgePrefab(const NoteShape shape) const
{
    if (const auto it = m_edgePrefabs.find(shape); it != m_edgePrefabs.end())
        return it->second;

    return nullptr;
}

void LongNote::SetEdgePrefab(const NoteShape shape, Gx::Sprite &prefab)
{
    m_edgePrefabs[shape] = &prefab;
}

void LongNote::Render(const ChartRenderer &renderer, const double delta)
{
    const double distance = GetRenderPosition() - renderer.GetRenderPosition();
    if (distance > 5.f || GetRenderPosition() + GetLength() - renderer.GetRenderPosition() < -0.5f)
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
    else
        SetVisible(true);

    GetGuideLine()->Render(renderer, delta);

    const auto edge = GetEdgePrefab(renderer.GetRenderSettings().Config->NoteShapeType);
    auto transform  = sprite->GetTransform();
    auto position   = transform.transformPoint(sf::Vector2f(0, renderer.MapRenderPositionToPixels(GetChannel(), distance + GetLength()) - 1));
    auto bounds     = transform.transformRect(sprite->GetLocalBounds());
    bounds.height   = renderer.MapRenderPositionToPixels(GetChannel(), GetLength(), true);

    UpdatePositions(GetVertices(), position, bounds);
    UpdateTexCoords(GetVertices(), sprite->GetTexCoords());

    if (edge)
    {
        transform = edge->GetTransform();
        position  = transform.transformPoint(sf::Vector2f(0, renderer.MapRenderPositionToPixels(GetChannel(), distance)));
        bounds    = transform.transformRect(edge->GetLocalBounds());

        UpdatePositions(GetTailVertices(), position, bounds);
        UpdateTexCoords(GetTailVertices(), edge->GetTexCoords());

        position = transform.transformPoint(sf::Vector2f(0, renderer.MapRenderPositionToPixels(GetChannel(), distance + GetLength())));
        UpdatePositions(GetHeadVertices(), position, bounds);
        UpdateTexCoords(GetHeadVertices(), edge->GetTexCoords());
    }
}
