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

    bool Control::IsEnabled() const
    {
        return m_enabled;
    }

    void Control::SetVisible(bool visible)
    {
        m_visible = visible;
        Invalidate();
    }

    bool Control::IsVislble() const
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
        OnControlStateChanged(this, m_state);
    }

    const sf::FloatRect Control::GetGlobalBounds() const
    {
        auto parent    = GetParent();
        auto transform = sf::Transform::Identity;
        while (parent)
        {
            transform *= parent->GetTransform();
            parent = parent->GetParent();
        }

        transform *= GetTransform();
        return transform.transformRect(GetLocalBounds());
    }

    void Control::SetClickCallback(std::function<void(Control*)> callback)
    {
        m_onClick = callback;
    }

    void Control::AddChild(Control *node)
    {
        if (!node)
            return;

        Node::AddChild(node);
        OnControlChildAdded(node);

        Invalidate();
    }

    void Control::RemoveChild(Control *node)
    {
        if (!node)
            return;

        OnControlChildRemove(node);
        Node::RemoveChild(node);
        Invalidate();
    }

    sf::RenderStates Control::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (!IsVislble())
            return states;

        states.transform *= GetTransform();
        return RenderableContainer::Render(target, states);
    }

    void Control::Update(double delta)
    {
        if (!IsEnabled())
            return;

        UpdatableContainer::Update(delta);
    }

    bool Control::Input(sf::Event ev)
    {
        if (!IsEnabled())
            return false;

        return InputableContainer::Input(ev);
    }

    void Control::OnMouseMove(sf::Event::MouseMoveEvent ev)
    {
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
        if (GetGlobalBounds().contains(ev.x, ev.y))
        {
            if (m_state == Control::State::Hover)
            {
                SetControlState(Control::State::Active);
                OnControlPress(this, ev);
            }
            else
                SetControlState(Control::State::Hover);
        }

        InputableContainer::OnMouseButtonClick(ev);
    }

    void Control::OnMouseButtonUp(sf::Event::MouseButtonEvent ev)
    {
        if (m_state == Control::State::Active)
        {
            SetControlState(Control::State::Normal);
            if (GetGlobalBounds().contains(ev.x, ev.y))
            {
                SetControlState(Control::State::Hover);
                OnControlClick(this, ev);

                if (m_onClick)
                    m_onClick(this);
            }
        }

        InputableContainer::OnMouseButtonUp(ev);
    }

    void Control::OnControlChildAdded(Control *control)
    {
    }

    void Control::OnControlChildRemove(Control *control)
    {
    }

    void Control::OnControlStateChanged(Control *sender, Control::State state)
    {
        Invalidate();

        auto parent = dynamic_cast<Control*>(GetParent());
        if (parent)
            parent->OnControlStateChanged(sender, state);
    }

    void Control::OnControlPress(Control *sender, sf::Event::MouseButtonEvent ev)
    {
        auto parent = dynamic_cast<Control*>(GetParent());
        if (parent)
            parent->OnControlPress(sender, ev);
    }

    void Control::OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev)
    {
        auto parent = dynamic_cast<Control*>(GetParent());
        if (parent)
            parent->OnControlClick(sender, ev);
    }
}
