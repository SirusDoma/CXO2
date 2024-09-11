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

    bool UiContainer::IsBatchingEnabled() const
    {
        return m_useBatching;
    }

    void UiContainer::SetBatchingEnabled(const bool batchingEnabled)
    {
        m_useBatching = batchingEnabled;
    }

    void UiContainer::OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev)
    {
        if (!IsEnabled())
            return;

        Control::OnControlClick(sender, ev);

        const auto radio = dynamic_cast<RadioButton *>(&sender);
        if (!radio || radio->IsChecked())
            return;

        m_activeRadio = radio;
    }

    void UiContainer::OnKeyPressed(const sf::Event::KeyPressed& ev)
    {
        Inputable::OnKeyPressed(ev);

        if (!IsEnabled() || ev.code != sf::Keyboard::Key::Tab)
            return;

        TextBox *first   = nullptr;
        TextBox *current = nullptr;

        for (const auto child : GetChildren())
        {
            const auto textBox = dynamic_cast<TextBox*>(child);
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

    RenderStates UiContainer::Render(RenderSurface &surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        if (m_useBatching)
        {
            states.transform *= GetTransform();
            return RenderBatchContainer::Render(surface, states);
        }

        return Control::Render(surface, states);
    }

    void UiContainer::Update(const double delta)
    {
        if (m_useBatching)
            RenderBatchContainer::Update(delta);
        else
            Control::Update(delta);
    }

    bool UiContainer::Input(const sf::Event& ev)
    {
        const bool input = Control::Input(ev);
        if (!m_activeRadio)
            return input;

        for (const auto child : GetChildren())
        {
            if (child == m_activeRadio)
                continue;

            const auto other = dynamic_cast<RadioButton *>(child);
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

        for (const auto node : GetChildren())
        {
            const auto control = dynamic_cast<Control*>(node);
            if (!control)
                continue;

            const auto bounds = control->GetGlobalBounds();
            if (first)
            {
                result.position.x = bounds.position.x;
                result.position.y  = bounds.position.y;
                first = false;
            }

            if (result.position.x > bounds.position.x)
                result.position.x = bounds.position.x;
            if (result.position.y  > bounds.position.y)
                result.position.y  = bounds.position.y;

            if (result.size.x  < bounds.position.x + bounds.size.x)
                result.size.x  = bounds.position.x + bounds.size.x;
            if (result.size.y < bounds.position.y  + bounds.size.y)
                result.size.y = bounds.position.y  + bounds.size.y;
        }

        m_localBounds = sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(result.size.x - result.position.x, result.size.y - result.position.y));
    }
}
