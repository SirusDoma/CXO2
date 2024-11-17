#include <Genode/UI/ToggleButton.hpp>

namespace Gx
{
    bool ToggleButton::IsChecked() const
    {
        return m_isChecked;
    }

    void ToggleButton::SetCheckedState(const bool checked)
    {
        if (IsChecked() != checked)
        {
            m_isChecked = checked;
            Invalidate();

            if (m_onCheckStateChanged)
                m_onCheckStateChanged(*this);
        }
    }

    void ToggleButton::SetCheckStateChangeCallback(std::function<void(ToggleButton&)> callback)
    {
        m_onCheckStateChanged = std::move(callback);
    }

    void ToggleButton::OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev)
    {
        if (!IsEnabled())
            return;

        Control::OnControlClick(sender, ev);
        if (&sender == this)
            SetCheckedState(!IsChecked());
    }

    Button::Frame ToggleButton::GetCurrentFrame() const
    {
        if (IsChecked())
            return GetStateFrame(State::Active);

        if (GetControlState() == State::Active)
            return GetStateFrame(State::Hover);

        return GetStateFrame(GetControlState());
    }
}
