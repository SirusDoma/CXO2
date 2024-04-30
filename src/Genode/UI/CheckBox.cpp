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

    void CheckBox::Invalidate()
    {
        if (!IsEnabled())
            return;

        Frame frame;
        if (IsChecked())
            frame = GetStateFrame(CheckBox::State::Active);
        else if (GetControlState() == CheckBox::State::Active)
            frame = GetStateFrame(CheckBox::State::Hover);
        else
            frame = GetStateFrame(GetControlState());

        ApplyFrame(frame);
    }
}
