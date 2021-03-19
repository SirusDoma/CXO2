#include <Genode/UI/UiContainer.hpp>
#include <Genode/UI/RadioButton.hpp>

namespace Gx
{
    UiContainer::UiContainer() :
        m_localBounds()
    {
    }

    UiContainer::~UiContainer()
    {
    }

    const sf::FloatRect UiContainer::GetLocalBounds() const
    {
        return m_localBounds;
    }

    void UiContainer::OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev)
    {
        Control::OnControlClick(sender, ev);

        auto radio = dynamic_cast<RadioButton*>(sender);
        if (!radio || !radio->IsChecked())
            return;

        for (auto child : GetChildren())
        {
            if (child == radio)
                continue;

            auto other = dynamic_cast<RadioButton*>(child);
            if (!other || other == radio)
                continue;

            other->SetCheckedState(false);
        }
    }

    sf::RenderStates UiContainer::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        return Control::Render(target, states);
    }

    void UiContainer::Update(double delta)
    {
        Control::Update(delta);
    }

    bool UiContainer::Input(sf::Event ev)
    {
        return Control::Input(ev);
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
