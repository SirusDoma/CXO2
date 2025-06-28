#include <OTwo/Core/LongNote.hpp>
#include <OTwo/Core/ChartRenderer.hpp>

LongNote::LongNote(const Chart::NoteEvent& ev) :
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
    if (!m_headVertices.has_value() || !m_tailVertices.has_value() || IsVisible() == visible)
        return;

    GetGuideLine()->SetVisible(visible);
    for (auto& v : m_headVertices.value())
        v.color = visible ? sf::Color::White : sf::Color::Transparent;

    for (auto& v : m_tailVertices.value())
        v.color = visible ? sf::Color::White : sf::Color::Transparent;

    for (auto& v : GetVertices())
        v.color = visible ? sf::Color::White : sf::Color::Transparent;
}

double LongNote::GetLength() const
{
    return m_length;
}

void LongNote::SetLength(const double length)
{
    m_length = length;
}

Gx::VertexSpan& LongNote::GetHeadVertices()
{
    return m_headVertices.value();
}

void LongNote::SetHeadVertices(Gx::VertexSpan&& vertices)
{
    m_headVertices = std::move(vertices);
}

Gx::VertexSpan& LongNote::GetTailVertices()
{
    return m_tailVertices.value();
}

void LongNote::SetTailVertices(Gx::VertexSpan&& vertices)
{
    m_tailVertices = std::move(vertices);
}

const Gx::Sprite* LongNote::GetEdgePrefab(const NoteShape shape) const
{
    if (const auto it = m_edgePrefabs.find(GetChannel()); it != m_edgePrefabs.end())
    {
        if (const auto it2 = it->second.find(shape); it2 != it->second.end())
            return it2->second;
    }

    return nullptr;
}

void LongNote::SetEdgePrefabs(const PrefabMap& prefabs)
{
    m_edgePrefabs = prefabs;
}

void LongNote::UpdateGeometry(const ChartRenderer& renderer, const double delta)
{
    const double distance = GetRenderPosition() - renderer.GetRenderPosition();
    if (distance > 5.f || GetRenderPosition() + GetLength() - renderer.GetRenderPosition() < -0.5f)
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

    const auto edge = GetEdgePrefab(renderer.GetRenderSettings().Config.NoteShapeType);
    auto transform  = sprite->GetTransform();
    auto position   = transform.transformPoint(sf::Vector2f(0, renderer.MapRenderPositionToPixels(GetChannel(), distance + GetLength()) - 1));
    auto bounds     = transform.transformRect(sprite->GetLocalBounds());
    bounds.size.y   = renderer.MapRenderPositionToPixels(GetChannel(), GetLength(), true);

    UpdatePositions(GetVertices(), position, bounds);
    UpdateTexCoords(GetVertices(), sprite->GetTexCoords());

    if (edge && m_headVertices.has_value() && m_tailVertices.has_value())
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
