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

    void CheckBox::OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev)
    {
        if (!IsEnabled())
            return;

        Control::OnControlClick(sender, ev);
        if (sender == this)
            SetCheckedState(!IsChecked());
    }

    Button::Frame CheckBox::GetCurrentFrame() const
    {
        if (IsChecked())
            return GetStateFrame(CheckBox::State::Active);

        if (GetControlState() == CheckBox::State::Active)
            return GetStateFrame(CheckBox::State::Hover);

        return GetStateFrame(GetControlState());
    }
}
