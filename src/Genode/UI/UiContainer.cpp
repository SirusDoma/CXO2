#include <Genode/UI/UiContainer.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/TextBox.hpp>

namespace Gx
{
    UiContainer::UiContainer() :
        m_localBounds(),
        m_activeRadio(),
        m_radioHandled(false)
    {
    }

    sf::FloatRect UiContainer::GetLocalBounds() const
    {
        return m_localBounds;
    }

    void UiContainer::SetRadioActiveCallback(std::function<void(RadioButton *)> callback)
    {
        m_radioCallback = std::move(callback);
    }

    void UiContainer::OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev)
    {
        if (!IsEnabled())
            return;

        Control::OnControlClick(sender, ev);

        auto radio = dynamic_cast<RadioButton *>(sender);
        if (!radio || radio->IsChecked())
            return;

        m_activeRadio = radio;
    }

    void UiContainer::OnKeyDown(sf::Event::KeyEvent ev)
    {
        Inputable::OnKeyDown(ev);

        if (!IsEnabled() || ev.code != sf::Keyboard::Tab)
            return;

        TextBox *first   = nullptr;
        TextBox *current = nullptr;

        for (auto child : GetChildren())
        {
            auto textBox = dynamic_cast<TextBox*>(child);
            if (!textBox || !textBox->IsEnabled())
                continue;

            if (current && !textBox->IsFocused())
            {
                current->SetFocus(false);
                textBox->SetFocus(true);
                return;
            }

            if (!first)
                first = textBox;

            if (textBox->IsFocused())
                current = textBox;
        }

        if (first && current)
        {
            current->SetFocus(false);
            first->SetFocus(true);
        }
    }

    RenderStates UiContainer::Render(sf::RenderTarget &target, RenderStates states) const
    {
        return Control::Render(target, states);
    }

    void UiContainer::Update(double delta)
    {
        Control::Update(delta);
    }

    bool UiContainer::Input(sf::Event ev)
    {
        bool input = Control::Input(ev);
        if (!m_activeRadio)
            return input;

        for (auto child : GetChildren())
        {
            if (child == m_activeRadio)
                continue;

            auto other = dynamic_cast<RadioButton *>(child);
            if (!other || other == m_activeRadio)
                continue;

            other->SetCheckedState(false);
        }

        m_activeRadio->SetCheckedState(true);
        if (m_radioCallback)
            m_radioCallback(m_activeRadio);

        m_activeRadio = nullptr;
        return input;
    }

    void UiContainer::Invalidate()
    {
        auto result = sf::FloatRect();
        bool first = true;

        for (auto node : GetChildren())
        {
            auto control = dynamic_cast<Control*>(node);
            if (!control)
                continue;

            auto bounds = control->GetGlobalBounds();
            if (first)
            {
                result.left = bounds.left;
                result.top  = bounds.top;
                first = false;
            }

            if (result.left > bounds.left)
                result.left = bounds.left;
            if (result.top  > bounds.top)
                result.top  = bounds.top;

            if (result.width  < bounds.left + bounds.width)
                result.width  = bounds.left + bounds.width;
            if (result.height < bounds.top  + bounds.height)
                result.height = bounds.top  + bounds.height;
        }

        m_localBounds = sf::FloatRect (0, 0, result.width - result.left, result.height - result.top);
    }
}
