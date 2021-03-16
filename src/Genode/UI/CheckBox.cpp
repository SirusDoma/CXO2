#include <Genode/UI/CheckBox.hpp>

namespace Gx
{
    const bool CheckBox::IsChecked() const
    {
        return m_isChecked;
    }

    void CheckBox::SetCheckedState(bool checked)
    {
        if (IsChecked() != checked)
        {
            m_isChecked = checked;
            Invalidate();

            if (m_onCheckStateChanged)
                m_onCheckStateChanged();
        }
    }

    void CheckBox::SetCheckStateChangeCallback(std::function<void()> callback)
    {
        m_onCheckStateChanged = callback;
    }

    sf::RenderStates CheckBox::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (!IsVislble())
            return states;

        auto frame = GetStateFrame(GetControlState());
        states.transform *= GetTransform();
        states.transform *= frame.GetTransform();
        target.draw(*GetSprite(), states);

        return RenderableContainer::Render(target, states);
    }

    void CheckBox::OnControlClick(sf::Event::MouseButtonEvent ev)
    {
        if (!IsEnabled())
            return;

        SetCheckedState(!IsChecked());
        Control::OnControlClick(ev);
    }

    void CheckBox::Invalidate()
    {
        Sprite frame;
        if (IsChecked())
            frame = GetStateFrame(CheckBox::State::Active);
        else if (GetControlState() == CheckBox::State::Active)
            frame = GetStateFrame(CheckBox::State::Hover);
        else
            frame = GetStateFrame(GetControlState());

        auto sprite = GetSprite();
        sprite->SetColor(frame.GetColor());
        sprite->SetTexCoords(frame.GetTexCoords());
    }
}