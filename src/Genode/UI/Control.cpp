#include <Genode/UI/Control.hpp>
#include <type_traits>

namespace Gx
{
    Control::Control() :
        m_enabled(true),
        m_visible(true),
        m_state(State::Normal)
    {
    }

    Control::~Control()
    {
    }

    void Control::SetEnabled(bool enabled)
    {
        m_enabled = enabled;
        Invalidate();
    }

    const bool &Control::IsEnabled() const
    {
        return m_enabled;
    }

    void Control::SetVisible(bool visible)
    {
        m_visible = visible;
        Invalidate();
    }

    const bool &Control::IsVislble() const
    {
        return m_visible;
    }

    const Control::State Control::GetControlState() const
    {
        return m_state;
    }

    void Control::SetControlState(const Control::State &state)
    {
        m_state = state;
        OnControlStateChanged(m_state);
    }

    const sf::FloatRect Control::GetGlobalBounds() const
    {
        return GetTransform().transformRect(GetLocalBounds());
    }

    void Control::SetClickCallback(std::function<void()> callback)
    {
        m_onClick = callback;
    }

    void Control::AddChild(Control *node)
    {
        if (!node)
            return;

        Node::AddChild(node);
        Invalidate();
    }

    void Control::RemoveChild(Control *node)
    {
        if (!node)
            return;

        Node::RemoveChild(node);
        Invalidate();
    }

    void Control::Update(double delta)
    {
        if (!IsEnabled())
            return;

        UpdatableContainer::Update(delta);
    }

    sf::RenderStates Control::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (!IsVislble())
            return states;

        states.transform *= GetTransform();
        return RenderableContainer::Render(target, states);
    }

    void Control::OnMouseMove(sf::Event::MouseMoveEvent ev)
    {
        if (!IsEnabled())
            return;

        if (GetControlState() != Control::State::Active)
        {
            bool intersect = GetGlobalBounds().contains(ev.x, ev.y);
            if (intersect && GetControlState() == Control::State::Normal)
                SetControlState(Control::State::Hover);
            else if (!intersect && GetControlState() == Control::State::Hover)
                SetControlState(Control::State::Normal);
        }

        InputableContainer::OnMouseMove(ev);
    }

    void Control::OnMouseButtonClick(sf::Event::MouseButtonEvent ev)
    {
        if (!IsEnabled())
            return;

        if (m_state == Control::State::Hover && GetGlobalBounds().contains(ev.x, ev.y))
        {
            SetControlState(Control::State::Active);
            OnControlPress(ev);
        }

        InputableContainer::OnMouseButtonClick(ev);
    }

    void Control::OnMouseButtonUp(sf::Event::MouseButtonEvent ev)
    {
        if (!IsEnabled() || m_state != Control::State::Active)
            return;

        SetControlState(Control::State::Normal);
        if (GetGlobalBounds().contains(ev.x, ev.y))
        {
            SetControlState(Control::State::Hover);
            OnControlClick(ev);

            if (m_onClick)
                m_onClick();
        }

        InputableContainer::OnMouseButtonUp(ev);
    }

    void Control::OnControlStateChanged(Control::State state)
    {
        Invalidate();
    }

    void Control::OnControlPress(sf::Event::MouseButtonEvent ev)
    {
    }

    void Control::OnControlClick(sf::Event::MouseButtonEvent ev)
    {
    }
}
