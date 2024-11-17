#include <Genode/UI/RadioButton.hpp>

#include <Genode/UI/UiContainer.hpp>

namespace Gx
{
    void RadioButton::SetCheckedState(const bool checked)
    {
        if (IsChecked() != checked)
        {
            ToggleButton::SetCheckedState(checked);
            if (const auto parent = GetParent(); parent && IsChecked())
            {
                for (const auto child : parent->GetChildren())
                {
                    const auto other = dynamic_cast<RadioButton*>(child);
                    if (!other || other == this)
                        continue;

                    other->SetCheckedState(false);
                }
            }

            if (m_onCheckStateChanged)
                m_onCheckStateChanged(*this);
        }
    }

    void RadioButton::OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev)
    {
        if (!IsEnabled())
            return;

        Control::OnControlClick(sender, ev);
        if (!IsChecked() && &sender == this)
            SetCheckedState(true);
    }

    void RadioButton::SetCheckStateChangeCallback(std::function<void(RadioButton&)> callback)
    {
        m_onCheckStateChanged = std::move(callback);
    }
}
