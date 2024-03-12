#include <Genode/UI/RadioButton.hpp>

#include <Genode/UI/UiContainer.hpp>

namespace Gx
{
    void RadioButton::SetCheckedState(bool checked)
    {
        if (IsChecked() != checked)
        {
            CheckBox::SetCheckedState(checked);
            if (IsChecked())
            {
                for (auto pair : m_pairs)
                {
                    if (pair && pair != this)
                        pair->SetCheckedState(false);
                }
            }

            if (m_onCheckStateChanged)
                m_onCheckStateChanged(this);
        }
    }

    void RadioButton::Pair(RadioButton *radio)
    {
        if (!radio)
            return;

        m_pairs.push_back(radio);
        radio->m_pairs.push_back(this);
    }

    void RadioButton::Unpair(RadioButton *radio)
    {
        for (auto pair : m_pairs)
        {
            pair->m_pairs.erase(
                std::remove_if(pair->m_pairs.begin(), pair->m_pairs.end(), [radio](auto r) { return radio == r; }),
                pair->m_pairs.end()
            );
        }

        m_pairs.erase(
            std::remove_if(m_pairs.begin(), m_pairs.end(), [radio](auto r) { return radio == r; }),
           m_pairs.end()
        );
    }

    void RadioButton::UnpairAll()
    {
        for (auto pair : m_pairs)
            pair->m_pairs.clear();

        m_pairs.clear();
    }

    void RadioButton::OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev)
    {
        if (!IsEnabled())
            return;

        Control::OnControlClick(sender, ev);
        if (!IsChecked() && sender == this)
            SetCheckedState(true);
    }

    void RadioButton::SetCheckStateChangeCallback(std::function<void(RadioButton*)> callback)
    {
        m_onCheckStateChanged = std::move(callback);
    }
}
