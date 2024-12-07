#include <Genode/UI/UiContainer.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/InputField.hpp>

namespace Gx
{
    UiContainer::UiContainer() :
        m_computedBounds(),
        m_localBounds()
    {
    }

    sf::FloatRect UiContainer::GetLocalBounds() const
    {
        if (m_localBounds != sf::FloatRect())
            return m_localBounds;

        return m_computedBounds;
    }

    void UiContainer::SetLocalBounds(const sf::FloatRect& bounds)
    {
        if (m_localBounds == bounds)
            return;

        m_localBounds = bounds;
        if (m_localBounds == sf::FloatRect())
            Invalidate();
    }

    bool UiContainer::IsBatchingEnabled() const
    {
        return m_useBatching;
    }

    void UiContainer::SetBatchingEnabled(const bool batchingEnabled)
    {
        m_useBatching = batchingEnabled;
    }


    void UiContainer::Apply(const std::function<void(Control&)>& fun) const
    {
        if (!fun)
            return;

        for (const auto child : GetChildren())
        {
            if (const auto control = dynamic_cast<Control*>(child))
                fun(*control);
        }
    }


    void UiContainer::OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev)
    {
        if (!IsEnabled())
            return;

        Control::OnControlClick(sender, ev);
    }

    void UiContainer::OnKeyPressed(const sf::Event::KeyPressed& ev)
    {
        Inputable::OnKeyPressed(ev);

        if (!IsEnabled() || ev.code != sf::Keyboard::Key::Tab)
            return;

        InputField* first   = nullptr;
        InputField* current = nullptr;

        for (const auto child : GetChildren())
        {
            const auto input = dynamic_cast<InputField*>(child);
            if (!input || !input->IsEnabled())
                continue;

            if (current && !input->IsFocused())
            {
                current->SetFocus(false);
                input->SetFocus(true);
                return;
            }

            if (!first)
                first = input;

            if (input->IsFocused())
                current = input;
        }

        if (first && current)
        {
            current->SetFocus(false);
            first->SetFocus(true);
        }
    }

    RenderStates UiContainer::Render(RenderSurface& surface, RenderStates states) const
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

    void UiContainer::Invalidate()
    {
        if (m_localBounds != sf::FloatRect())
            return;

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

        m_computedBounds = sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(result.size.x - result.position.x, result.size.y - result.position.y));
    }
}
