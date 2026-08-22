#include <CXO2/UI/RadioButton.hpp>

#include <CXO2/UI/UiContainer.hpp>

namespace Cx
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

            if (IsEnabled() && m_onCheckStateChanged)
            {
                auto uiEvent = Event{false, GetControlState()};
                m_onCheckStateChanged(*this, uiEvent);
            }
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

    void RadioButton::SetCheckStateChangeCallback(std::function<void(RadioButton&, Control::Event&)> callback)
    {
        m_onCheckStateChanged = std::move(callback);
    }
}
