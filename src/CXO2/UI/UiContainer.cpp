#include <CXO2/UI/UiContainer.hpp>
#include <CXO2/UI/RadioButton.hpp>
#include <CXO2/UI/InputField.hpp>

namespace Cx
{
    UiContainer::UiContainer() :
        m_computedLocalBounds(),
        m_localBounds()
    {
    }

    sf::FloatRect UiContainer::GetGlobalBounds() const
    {
        if (m_localBounds != sf::FloatRect())
            return Control::GetGlobalBounds();

        return m_computedGlobalBounds;
    }

    sf::FloatRect UiContainer::GetLocalBounds() const
    {
        if (m_localBounds != sf::FloatRect())
            return m_localBounds;

        return m_computedLocalBounds;
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
        Gx::Inputable::OnKeyPressed(ev);

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

    Gx::RenderStates UiContainer::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        if (!IsVisible())
            return states;

        if (m_useBatching)
        {
            states.transform *= GetTransform();
            return Gx::RenderBatchContainer::Render(surface, states);
        }

        return Control::Render(surface, states);
    }

    void UiContainer::Update(const sf::Time& delta)
    {
        if (m_useBatching)
            Gx::RenderBatchContainer::Update(delta);
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

            if (const auto container = dynamic_cast<UiContainer*>(control))
                container->Invalidate();

            const auto bounds = control->GetGlobalBounds();
            if (first)
            {
                result.position.x = bounds.position.x;
                result.position.y = bounds.position.y;
                first = false;
            }

            if (result.position.x > bounds.position.x)
                result.position.x = bounds.position.x;
            if (result.position.y > bounds.position.y)
                result.position.y = bounds.position.y;

            if (result.size.x < bounds.position.x + bounds.size.x)
                result.size.x = bounds.position.x + bounds.size.x;
            if (result.size.y < bounds.position.y + bounds.size.y)
                result.size.y = bounds.position.y + bounds.size.y;
        }

        m_computedLocalBounds = sf::FloatRect(sf::Vector2f(0, 0),
            sf::Vector2f(result.size.x - result.position.x, result.size.y - result.position.y));

        m_computedGlobalBounds = sf::FloatRect(result.position, m_computedLocalBounds.size);
    }
}
