#include <OTwo/Chart/LongNote.hpp>

LongNote::LongNote(const ChartRenderer &renderer, const Chart::NoteEvent &ev, const NoteSpriteMap &sprites, const NoteSpriteMap &heads) :
    LongNote::LongNote(renderer, ev.Channel, ev.Position, ev.Length, sprites, heads)
{

}

LongNote::LongNote(const ChartRenderer &renderer, const Chart::Channel channel, const double position, const double length, const NoteSpriteMap &sprites, const NoteSpriteMap &heads) :
    Note::Note(renderer, channel, position),
    m_position(position),
    m_length(length),
    m_sprites(sprites),
    m_heads(heads),
    m_visible(true),
    m_accuracy(Accuracy::None)
{
    m_config = m_renderer->GetRenderSettings().Config;
    const auto sprite     = m_sprites[m_config->NoteShapeType];
    const double distance = m_renderer->MapRenderPositionToPixels(m_channel, m_length, true);

    SetScale(GetScale().x, distance / sprite->GetLocalBounds().height);
}

bool LongNote::IsVisible() const
{
    return m_visible;
}

void LongNote::SetVisible(const bool visible)
{
    m_visible = visible;
}

Gx::RenderStates LongNote::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    if (!IsVisible() || !m_renderer->InRenderProximity(m_position + m_length))
        return states;

    auto cstates = states;
    states.transform *= GetTransform();
    if (const auto it = m_sprites.find(m_config->NoteShapeType); it != m_sprites.end())
        surface.Render(*it->second, states);

    // For heads and tails: only apply position, ignore scale
    cstates.transform.translate(GetPosition());
    if (const auto it = m_heads.find(m_config->NoteShapeType); it != m_heads.end())
    {
        // Tail
        surface.Render(*it->second, cstates);

        // TODO: GRID LENGTH
        const auto length = m_config->NoteGuideLength * (m_renderer->GetBPM() / 60.f * 5.5f) *  m_renderer->GetSpeed(m_channel);
        const auto x1 = it->second->GetPosition().x;
        const auto x2 = x1 + 1;
        const auto y1 = 0;
        const auto y2 = length;

        if (length > 0)
        {
            // Tail grid
            auto tailGrid = sf::VertexArray(sf::PrimitiveType::TriangleStrip);
            tailGrid.append({ sf::Vector2f(x1, -y2), sf::Color::Black });
            tailGrid.append({ sf::Vector2f(x2, -y2), sf::Color::Black });
            tailGrid.append({ sf::Vector2f(x1,  y1), sf::Color(125, 125, 125) });
            tailGrid.append({ sf::Vector2f(x2,  y1), sf::Color(125, 125, 125) });
            surface.Render(tailGrid, states);

            for (int i = 0; i < tailGrid.getVertexCount(); i++)
                tailGrid[i].position.x += it->second->GetLocalBounds().width;

            surface.Render(tailGrid, states);
        }

        // Head
        const double distance = m_renderer->MapRenderPositionToPixels(m_channel, m_length, true);
        cstates.transform.translate({0, static_cast<float>(distance)});
        surface.Render(*it->second, cstates);

        if (length > 0)
        {
            // Head grid
            auto headGrid = sf::VertexArray(sf::PrimitiveType::TriangleStrip);
            headGrid.append({ sf::Vector2f(x1,  y1), sf::Color(125, 125, 125) });
            headGrid.append({ sf::Vector2f(x2,  y1), sf::Color(125, 125, 125) });
            headGrid.append({ sf::Vector2f(x1,  y2), sf::Color::Black });
            headGrid.append({ sf::Vector2f(x2,  y2), sf::Color::Black });
            surface.Render(headGrid, cstates);

            for (int i = 0; i < headGrid.getVertexCount(); i++)
                headGrid[i].position.x += it->second->GetLocalBounds().width;

            surface.Render(headGrid, cstates);
        }
    }

    return RenderableContainer::Render(surface, states);
}

void LongNote::Update(const double delta)
{
    const double position = (m_position + m_length) - m_renderer->GetRenderPosition();
    SetPosition(GetPosition().x, m_renderer->MapRenderPositionToPixels(m_channel, position));

    UpdatableContainer::Update(delta);
}
