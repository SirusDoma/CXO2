#include <OTwo/Chart/LongNote.hpp>

LongNote::LongNote(const ChartRenderer &renderer, const Chart::NoteEvent &ev, const NoteSpriteMap &sprites, const NoteSpriteMap &heads) :
    LongNote::LongNote(renderer, ev.Channel, ev.Position, ev.Length, sprites, heads)
{

}

LongNote::LongNote(const ChartRenderer &renderer, const Chart::Channel channel, const double position, const double length, const NoteSpriteMap &sprites, const NoteSpriteMap &heads) :
    m_renderer(&renderer),
    m_channel(channel),
    m_position(position),
    m_length(length),
    m_sprites(sprites),
    m_heads(heads),
    m_visible(true),
    m_accuracy(Accuracy::None)
{
    m_config = m_renderer->GetContext().GetConfig();

    const auto sprite     = m_sprites[m_config->NoteShapeType];
    const double speed    = m_renderer->GetSpeed(m_channel);
    const double distance = m_length * (static_cast<float>(ChartRenderer::DefaultMeasureHeight) * speed);

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

Accuracy LongNote::GetJudgementAccuracy() const
{
    return m_accuracy;
}

void LongNote::Judge(const Accuracy accuracy)
{
    if (accuracy == Accuracy::None || m_accuracy != Accuracy::None)
        return;

    m_accuracy = accuracy;
}

Gx::RenderStates LongNote::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    if (!IsVisible() || (m_position + m_length) - m_renderer->GetRenderPosition() < -0.5)
        return states;

    auto cstates = states;
    states.transform *= GetTransform();
    if (const auto it = m_sprites.find(m_config->NoteShapeType); it != m_sprites.end())
        surface.Render(*it->second, states);

    // For heads and tails: only apply position, ignore scale
    cstates.transform.translate(GetPosition());
    const auto speed = m_renderer->GetSpeed(m_channel);
    if (const auto it = m_heads.find(m_config->NoteShapeType); it != m_heads.end())
    {
        // Tail
        surface.Render(*it->second, cstates);

        // Head
        const double distance = m_length * (static_cast<float>(ChartRenderer::DefaultMeasureHeight) * speed);
        cstates.transform.translate({0, static_cast<float>(distance)});
        surface.Render(*it->second, cstates);
    }

    return RenderableContainer::Render(surface, states);
}

void LongNote::Update(const double delta)
{
    const auto context  = &m_renderer->GetContext();
    const auto speed    = m_renderer->GetSpeed(m_channel);
    const auto position = ((m_position + m_length) - m_renderer->GetRenderPosition()) * (static_cast<float>(ChartRenderer::DefaultMeasureHeight) * speed);
    SetPosition(GetPosition().x, context->GetViewport() - position);

    UpdatableContainer::Update(delta);
}
