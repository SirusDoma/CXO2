#include <Genode/Graphics/Shapes/Polygon.hpp>
#include <OTwo/Chart/Note.hpp>

Note::Note(const ChartRenderer &renderer, const Chart::Event &ev, const NoteSpriteMap &sprites) :
    Note::Note(renderer, ev.Channel, ev.Position, sprites)
{

}

Note::Note(const ChartRenderer &renderer, const Chart::Channel channel, const double position, const NoteSpriteMap &sprites) :
    m_renderer(&renderer),
    m_channel(channel),
    m_position(position),
    m_sprites(sprites),
    m_visible(true),
    m_accuracy(Accuracy::None)
{
    m_config = m_renderer->GetRenderSettings().Config;
}

Note::Note(const ChartRenderer &renderer, const Chart::Channel channel, const double position) :
    m_renderer(&renderer),
    m_channel(channel),
    m_position(position),
    m_config(nullptr),
    m_visible(true),
    m_accuracy(Accuracy::None)
{
    m_config = m_renderer->GetRenderSettings().Config;
}

bool Note::IsVisible() const
{
    return m_visible && m_accuracy == Accuracy::None;
}

void Note::SetVisible(const bool visible)
{
    m_visible = visible;
}

Accuracy Note::GetJudgementAccuracy() const
{
    return m_accuracy;
}

void Note::Judge(const Accuracy accuracy)
{
    if (accuracy == Accuracy::None || m_accuracy != Accuracy::None)
        return;

    m_accuracy = accuracy;
}


Gx::RenderStates Note::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    if (const auto position = GetPosition(); !IsVisible() || position.y < -50 || position.y > m_renderer->GetRenderSettings().Viewport + 50)
        return states;

    states.transform *= GetTransform();
    if (const auto it = m_sprites.find(m_config->NoteShapeType); it != m_sprites.end())
    {
        surface.Render(*it->second, states);

        if (m_channel != Chart::Channel::BGM)
        {
            // TODO: GRID LENGTH
            const auto length = m_config->NoteGuideLength * (m_renderer->GetBPM() / 60.f * 5.5f) *  m_renderer->GetSpeed(m_channel);
            const auto x1 = it->second->GetPosition().x;
            const auto x2 = x1 + 1;

            auto grid = sf::VertexArray(sf::PrimitiveType::TriangleStrip);
            grid.append({ sf::Vector2f(x1, -length), sf::Color::Black });
            grid.append({ sf::Vector2f(x2, -length), sf::Color::Black });
            grid.append({ sf::Vector2f(x1, 0), sf::Color(125, 125, 125) });
            grid.append({ sf::Vector2f(x2, 0), sf::Color(125, 125, 125) });
            grid.append({ sf::Vector2f(x1, length), sf::Color::Black });
            grid.append({ sf::Vector2f(x2, length), sf::Color::Black });
            surface.Render(grid, states);

            for (int i = 0; i < grid.getVertexCount(); i++)
                grid[i].position.x += it->second->GetLocalBounds().width;

            surface.Render(grid, states);
        }
    }

    return RenderableContainer::Render(surface, states);
}

void Note::Update(const double delta)
{
    const double position = m_position - m_renderer->GetRenderPosition();
    SetPosition(GetPosition().x, m_renderer->MapRenderPositionToPixels(m_channel, position));

    UpdatableContainer::Update(delta);
}
