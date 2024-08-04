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
    m_config = m_renderer->GetContext().GetConfig();
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
    const auto context = &m_renderer->GetContext();
    if (const auto position = GetPosition(); !IsVisible() || position.y < -50 || position.y > context->GetViewport() + 50)
        return states;

    states.transform *= GetTransform();
    if (const auto it = m_sprites.find(m_config->NoteShapeType); it != m_sprites.end())
        surface.Render(*it->second, states);

    return RenderableContainer::Render(surface, states);
}

void Note::Update(const double delta)
{
    const auto context  = &m_renderer->GetContext();
    const auto speed    = m_renderer->GetSpeed(m_channel);
    const auto position = (m_position - m_renderer->GetRenderPosition()) * (static_cast<float>(ChartRenderer::DefaultMeasureHeight) * speed);
    SetPosition(GetPosition().x, context->GetViewport() - position);

    UpdatableContainer::Update(delta);
}
