#include <Genode/UI/RadioButton.hpp>

#include <Genode/UI/Container.hpp>

namespace Gx
{
    void RadioButton::SetCheckedState(bool checked)
    {
        bool alreadyChecked = IsChecked();
        if (alreadyChecked != checked)
        {
            CheckBox::SetCheckedState(checked);
            if (checked)
            {
                for (auto pair : m_pairs)
                {
                    if (pair && pair != this)
                        pair->SetCheckedState(false);
                }
            }
        }
    }

    void RadioButton::Pair(RadioButton *radio)
    {
        if (!radio)
            return;

        m_pairs.push_back(radio);
        for (auto pair : radio->m_pairs)
            m_pairs.push_back(pair);

        radio->m_pairs.push_back(this);
        for (auto pair : m_pairs)
        {
            pair->m_pairs.push_back(radio);
            radio->m_pairs.push_back(pair);
        }
    }

    void RadioButton::UnpairAll()
    {
        for (auto pair : m_pairs)
            pair->m_pairs.clear();

        m_pairs.clear();
    }

    void RadioButton::OnControlClick(sf::Event::MouseButtonEvent ev)
    {
        if (!IsEnabled())
            return;

        if (!IsChecked())
            SetCheckedState(true);

        Control::OnControlClick(ev);
    }
}