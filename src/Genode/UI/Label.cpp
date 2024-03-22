#include <Genode/UI/Label.hpp>

namespace Gx
{
    sf::FloatRect Label::GetLocalBounds() const
    {
        return Text::GetLocalBounds();
    }

    void Label::Update(double delta)
    {
        Control::Update(delta);

        if (!m_alignmentUpdated)
            Invalidate();
    }

    sf::RenderStates Label::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (!IsVislble())
            return states;

        return Text::Render(target, states);
    }

    Label::Alignment Label::GetAlignment() const
    {
        return m_alignment;
    }

    void Label::SetAlignment(Label::Alignment alignment)
    {
        if (m_alignment == alignment)
            return;

        m_alignment = alignment;
        OnGeometryUpdated();
    }

    void Label::OnGeometryUpdated() const
    {
        Text::OnGeometryUpdated();
        m_alignmentUpdated = false;
    }

    void Label::Invalidate()
    {
        auto bounds = GetLocalBounds();
        auto origin = GetOrigin();

        if (m_alignment == Alignment::Left)
            SetOrigin(bounds.left, origin.y);
        else if (m_alignment == Alignment::Center)
            SetOrigin(std::floor(bounds.left + (bounds.width / 2.f)), origin.y);
        else if (m_alignment == Alignment::Right)
            SetOrigin((bounds.left + bounds.width), origin.y);

        m_alignmentUpdated = true;
    }
}
