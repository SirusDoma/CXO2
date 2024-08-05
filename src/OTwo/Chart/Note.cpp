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
        surface.Render(*it->second, states);

    return RenderableContainer::Render(surface, states);
}

void Note::Update(const double delta)
{
    const double position = m_position - m_renderer->GetRenderPosition();
    SetPosition(GetPosition().x, m_renderer->MapRenderPositionToPixels(m_channel, position));

    UpdatableContainer::Update(delta);
}
