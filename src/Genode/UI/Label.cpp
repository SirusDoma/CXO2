#include <Genode/UI/Label.hpp>

namespace Gx
{
    sf::FloatRect Label::GetLocalBounds() const
    {
        return Text::GetLocalBounds();
    }

    void Label::Update(const double delta)
    {
        Control::Update(delta);

        Invalidate();
    }

    RenderStates Label::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        return Text::Render(surface, states);
    }

    Label::Alignment Label::GetAlignment() const
    {
        return m_alignment;
    }

    void Label::SetAlignment(Alignment alignment)
    {
        if (m_alignment == alignment)
            return;

        m_alignment = alignment;
        m_alignmentUpdated = false;
        Invalidate();
    }

    void Label::OnGeometryUpdated() const
    {
        Text::OnGeometryUpdated();
        m_alignmentUpdated = false;
    }

    void Label::Invalidate()
    {
        EnsureGeometryUpdate();

        if (m_alignmentUpdated)
            return;

        const auto bounds = GetLocalBounds();
        const auto origin = GetOrigin();

        if (m_alignment == Alignment::Left)
            SetOrigin(bounds.position.x, origin.y);
        else if (m_alignment == Alignment::Center)
            SetOrigin(std::floor(bounds.position.x + (bounds.size.x / 2.f)), origin.y);
        else if (m_alignment == Alignment::Right)
            SetOrigin((bounds.position.x + bounds.size.x), origin.y);

        m_alignmentUpdated = true;
    }
}
