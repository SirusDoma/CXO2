#include <Genode/UI/CheckBox.hpp>

namespace Gx
{
    const bool CheckBox::IsChecked() const
    {
        return m_isChecked;
    }

    void CheckBox::SetCheckedState(bool checked)
    {
        m_isChecked = checked;
    }

    sf::RenderStates CheckBox::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (!IsVislble())
            return states;

        auto frame = GetStateFrame(GetState());
        if (IsChecked())
            frame = GetStateFrame(CheckBox::State::Pressed);
        else if (GetState() == CheckBox::State::Pressed)
            frame = GetStateFrame(CheckBox::State::Hover);

        auto sprite = GetSprite();
        sprite->SetColor(frame.GetColor());
        sprite->SetTexCoords(frame.GetTexCoords());

        states.transform *= GetTransform() * frame.GetTransform();
        target.draw(*sprite, states);

        return states;
    }

    void CheckBox::OnMouseButtonUp(sf::Event::MouseButtonEvent ev)
    {
        if (!IsEnabled() || GetState() != CheckBox::State::Pressed)
            return;

        Button::OnMouseButtonUp(ev);
        if (IsIntersect(sf::Vector2f(ev.x, ev.y)))
            m_isChecked = !m_isChecked;
    }
}