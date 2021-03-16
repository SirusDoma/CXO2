#include <Genode/UI/UiContainer.hpp>
#include <Genode/UI/RadioButton.hpp>

namespace Gx
{
    UiContainer::UiContainer() :
        m_localBounds(),
        m_overlay(),
        m_backDrop(),
        m_radio()
    {
        m_backDrop.SetColor(sf::Color(0, 0, 0, 255 / 2));
    }

    UiContainer::~UiContainer()
    {
        if (m_overlay)
            delete m_overlay;
    }

    const sf::FloatRect UiContainer::GetLocalBounds() const
    {
        return m_localBounds;
    }

    Control *UiContainer::GetOverlay() const
    {
        return m_overlay;
    }

    void UiContainer::SetOverlay(Control *control)
    {
        m_overlay = control;
    }

    void UiContainer::CloseOverlay(bool cleanup)
    {
        if (m_overlay && cleanup)
            delete m_overlay;

        m_overlay = nullptr;
    }

    const sf::Color UiContainer::GetBackdropColor() const
    {
        return m_backDrop.GetColor();
    }

    void UiContainer::SetBackdropColor(const sf::Color &color)
    {
        m_backDrop.SetColor(color);
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
        if (!IsVislble())
            return states;

        states = Control::Render(target, states);
        if (m_overlay)
        {
            target.draw(m_backDrop, states);
            m_overlay->Render(target, states);
        }

        return states;
    }

    void UiContainer::Update(double delta)
    {
        if (!IsEnabled() || m_overlay)
            return;

        Control::Update(delta);
    }

    bool UiContainer::Input(sf::Event ev)
    {
        if (!IsEnabled() || m_overlay)
            return false;

        return Control::Input(ev);
    }

    void UiContainer::Invalidate()
    {
        auto result = sf::FloatRect();
        bool first = true;

        for (auto node : GetChildren())
        {
            auto control = dynamic_cast<Control*>(node);
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
        m_backDrop.SetSize(sf::Vector2f(m_localBounds.width, m_localBounds.height));
    }
}
