#include <Genode/UI/Control.hpp>
#include <type_traits>

namespace Gx
{
    Control::Control() :
        m_enabled(true),
        m_visible(true),
        m_focused(false),
        m_state(State::Normal),
        m_onClick(),
        m_onGainFocus(),
        m_onLostFocus()
    {
    }

    Control::~Control()
    {
    }

    bool Control::IsFocused() const
    {
        return m_focused;
    }

    void Control::SetFocus(bool focus)
    {
        if (m_focused == focus || m_state == Control::State::Active)
            return;

        m_focused = focus;
        auto uiEvent = Event{false, GetControlState()};
        if (m_onFocusChanged)
            m_onFocusChanged(*this, uiEvent);

        if (m_focused && m_onGainFocus)
            m_onGainFocus(*this, uiEvent);
        else if (!m_focused && m_onLostFocus)
            m_onLostFocus(*this, uiEvent);

        SetControlState(uiEvent.State);
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
        if (m_state != state)
        {
            m_state = state;
            SetFocus(m_state == Control::State::Hover || m_state == Control::State::Active);
            if (IsEnabled())
                OnControlStateChanged(this, m_state);
        }
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

    void Control::SetFocusChangedCallback(std::function<void(Control&, Control::Event&)> callback)
    {
        m_onFocusChanged = callback;
    }

    void Control::SetGainFocusCallback(std::function<void(Control&, Control::Event&)> callback)
    {
        m_onGainFocus = callback;
    }

    void Control::SetLostFocusCallback(std::function<void(Control&, Control::Event&)> callback)
    {
        m_onLostFocus = callback;
    }

    void Control::SetClickCallback(std::function<void(Control&, Event&)> callback)
    {
        m_onClick = callback;
    }

    const std::function<void(Control&, Control::Event&)> &Control::GetFocusChangedCallback()
    {
        return m_onFocusChanged;
    }

    const std::function<void(Control&, Control::Event&)> &Control::GetGainFocusCallback()
    {
        return m_onGainFocus;
    }

    const std::function<void(Control&, Control::Event&)> &Control::GetLostFocusCallback()
    {
        return m_onLostFocus;
    }

    const std::function<void(Control&, Control::Event&)> &Control::GetClickCallback()
    {
        return m_onClick;
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
        {
            if (ev.type == sf::Event::MouseMoved)
            {
                OnMouseMove(ev.mouseMove);
                return true;
            }

            return false;
        }

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

        if (!IsEnabled())
            return;

        InputableContainer::OnMouseMove(ev);
    }

    void Control::OnMouseButtonDown(sf::Event::MouseButtonEvent ev)
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

        InputableContainer::OnMouseButtonDown(ev);
    }

    void Control::OnMouseButtonUp(sf::Event::MouseButtonEvent ev)
    {
        if (GetGlobalBounds().contains(ev.x, ev.y))
        {
            if (m_state == Control::State::Active)
            {
                if (m_onClick)
                {
                    auto uiEvent = Event{false, Control::State::Hover};
                    m_onClick(*this, uiEvent);

                    SetControlState(uiEvent.State);
                    if (uiEvent.Handled)
                        return;
                }
                else
                    SetControlState(Control::State::Hover);

                OnControlClick(this, ev);
            }
            else
                SetControlState(Control::State::Hover);
        }
        else
            SetControlState(Control::State::Normal);

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

    void Control::OnMouseWheelScrolled(sf::Event::MouseWheelScrollEvent ev)
    {
        Inputable::OnMouseWheelScrolled(ev);
    }
}
