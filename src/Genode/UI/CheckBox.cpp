#include <Genode/UI/CheckBox.hpp>

namespace Gx
{
    bool CheckBox::IsChecked() const
    {
        return m_isChecked;
    }

    void CheckBox::SetCheckedState(const bool checked)
    {
        if (IsChecked() != checked)
        {
            m_isChecked = checked;
            Invalidate();

            if (m_onCheckStateChanged)
                m_onCheckStateChanged(this);
        }
    }

    void CheckBox::SetCheckStateChangeCallback(std::function<void(CheckBox*)> callback)
    {
        m_onCheckStateChanged = std::move(callback);
    }

    void CheckBox::OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev)
    {
        if (!IsEnabled())
            return;

        Control::OnControlClick(sender, ev);
        if (&sender == this)
            SetCheckedState(!IsChecked());
    }

    Button::Frame CheckBox::GetCurrentFrame() const
    {
        if (IsChecked())
            return GetStateFrame(State::Active);

        if (GetControlState() == State::Active)
            return GetStateFrame(State::Hover);

        return GetStateFrame(GetControlState());
    }
}
