#include <Genode/UI/Control.hpp>
#include <type_traits>

namespace Gx
{
    Control::Control() :
        m_state(State::Normal),
        m_enabled(true),
        m_focused(false),
        m_clicked(false),
        m_doubleClicked(false),
        m_deltaClickDuration(),
        m_deltaHoldDuration(),
        m_onClick(),
        m_onHoldClick(),
        m_onDoubleClick(),
        m_onScrollWheel(),
        m_onGainFocus(),
        m_onLostFocus()
    {
    }

    bool Control::IsFocused() const
    {
        return m_focused;
    }

    void Control::SetFocus(const bool focus)
    {
        if (m_focused == focus || m_state == Control::State::Active)
            return;

        m_focused = focus;
        auto uiEvent = Event{false, m_focused ? Control::State::Hover : Control::State::Normal};
        if (m_onFocusChanged)
            m_onFocusChanged(*this, uiEvent);

        if (m_focused && m_onGainFocus)
            m_onGainFocus(*this, uiEvent);
        else if (!m_focused && m_onLostFocus)
            m_onLostFocus(*this, uiEvent);

        SetControlState(uiEvent.State);
    }

    void Control::SetEnabled(const bool enabled)
    {
        if (m_enabled != enabled)
        {
            m_enabled = enabled;
            Invalidate();
        }
    }

    bool Control::IsEnabled() const
    {
        return m_enabled;
    }

    void Control::SetVisible(const bool visible)
    {
        if (Renderable::IsVisible() != visible)
        {
            Renderable::SetVisible(visible);
            Invalidate();
        }
    }

    Control::State Control::GetControlState() const
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
                OnControlStateChanged(*this, m_state);
        }
    }

    sf::FloatRect Control::GetGlobalBounds() const
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
        m_onFocusChanged = std::move(callback);
    }

    void Control::SetGainFocusCallback(std::function<void(Control&, Control::Event&)> callback)
    {
        m_onGainFocus = std::move(callback);
    }

    void Control::SetLostFocusCallback(std::function<void(Control&, Control::Event&)> callback)
    {
        m_onLostFocus = std::move(callback);
    }

    void Control::SetClickCallback(std::function<void(Control&, Control::Event&)> callback)
    {
        m_onClick = std::move(callback);
    }

    void Control::SetHoldClickCallback(std::function<void(Control &, Event &)> callback)
    {
        m_onHoldClick = std::move(callback);
    }

    void Control::SetDoubleClickCallback(std::function<void(Control&, Control::Event&)> callback)
    {
        m_onDoubleClick = std::move(callback);
    }

    void Control::SetScrollWheelCallback(std::function<void(Control &, Event &)> callback)
    {
        m_onScrollWheel = std::move(callback);
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

    const std::function<void(Control&, Control::Event&)> &Control::GetHoldClickCallback()
    {
        return m_onHoldClick;
    }

    const std::function<void(Control&, Control::Event&)> &Control::GetDoubleClickCallback()
    {
        return m_onDoubleClick;
    }

    const std::function<void(Control &, Control::Event&)> &Control::GetScrollWheelCallback()
    {
        return m_onScrollWheel;
    }

    void Control::AddChild(Control *node)
    {
        if (!node)
            return;

        Node::AddChild(node);
        OnControlChildAdded(*node);

        Invalidate();
    }

    void Control::AddChild(Gx::Node *node)
    {
        if (const auto control = dynamic_cast<Gx::Control*>(node); control)
            AddChild(control);
        else
            Node::AddChild(node);
    }

    void Control::RemoveChild(Control *node)
    {
        if (!node)
            return;

        OnControlChildRemove(*node);
        Node::RemoveChild(node);
        Invalidate();
    }

    void Control::RemoveChild(Gx::Node *node)
    {
        if (const auto control = dynamic_cast<Gx::Control*>(node); control)
            RemoveChild(control);
        else
            Node::RemoveChild(node);
    }

    RenderStates Control::Render(RenderSurface &surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        states.transform *= GetTransform();
        return RenderableContainer::Render(surface, states);
    }

    void Control::Update(const double delta)
    {
        if (!IsEnabled())
            return;

        if (m_clicked)
        {
            m_deltaClickDuration += delta;
            if (m_deltaClickDuration > DOUBLE_CLICK_THRESHOLD)
            {
                m_clicked = false;
                m_deltaClickDuration = 0;
            }
        }

        if (GetControlState() == Control::State::Active && m_onHoldClick)
        {
            m_deltaHoldDuration += delta;
            if (m_deltaHoldDuration >= HOLD_CLICK_THRESHOLD)
            {
                auto uiEvent = Event{false, GetControlState()};
                m_onHoldClick(*this, uiEvent);

                m_deltaHoldDuration = 0;
            }
        }
        else
            m_deltaHoldDuration = 0;

        UpdatableContainer::Update(delta);
    }

    bool Control::Input(const sf::Event& ev)
    {
        if (!IsEnabled())
        {
            if (const auto mouseMove = ev.getIf<sf::Event::MouseMoved>())
            {
                OnMouseMoved(*mouseMove);
                return true;
            }

            return false;
        }

        return InputableContainer::Input(ev);
    }

    void Control::OnMouseMoved(const sf::Event::MouseMoved& ev)
    {
        if (GetControlState() != Control::State::Active)
        {
            const bool intersect = GetGlobalBounds().contains(sf::Vector2f(ev.position.x, ev.position.y));
            if (intersect && GetControlState() == Control::State::Normal)
                SetControlState(Control::State::Hover);
            else if (!intersect && GetControlState() == Control::State::Hover)
                SetControlState(Control::State::Normal);
        }

        if (!IsEnabled())
            return;

        InputableContainer::OnMouseMoved(ev);
    }

    void Control::OnMouseButtonPressed(const sf::Event::MouseButtonPressed& ev)
    {
        if (GetGlobalBounds().contains(sf::Vector2f(ev.position.x, ev.position.y)))
        {
            if (GetControlState() == Control::State::Hover)
            {
                SetControlState(Control::State::Active);
                OnControlPress(*this, ev);

                if (m_clicked)
                {
                    m_clicked = false;
                    if (m_deltaClickDuration <= DOUBLE_CLICK_THRESHOLD)
                    {
                        if (m_onDoubleClick)
                        {
                            auto uiEvent = Event{false, GetControlState()};
                            m_doubleClicked = true;
                            m_onDoubleClick(*this, uiEvent);

                            SetControlState(uiEvent.State);
                            if (uiEvent.Handled)
                                return;
                        }

                        OnControlDoubleClick(*this, ev);
                    }
                }
                else
                    m_clicked = true;

                m_deltaClickDuration = 0;
            }
            else
                SetControlState(Control::State::Hover);
        }

        InputableContainer::OnMouseButtonPressed(ev);
    }

    void Control::OnMouseButtonReleased(const sf::Event::MouseButtonReleased& ev)
    {
        if (GetGlobalBounds().contains(sf::Vector2f(ev.position.x, ev.position.y)))
        {
            if (GetControlState() == Control::State::Active)
            {
                if (m_onClick && !m_doubleClicked)
                {
                    auto uiEvent = Event{false, Control::State::Hover};
                    m_onClick(*this, uiEvent);

                    SetControlState(uiEvent.State);
                    if (uiEvent.Handled)
                        return;
                }
                else
                    SetControlState(Control::State::Hover);

                OnControlClick(*this, ev);
            }
            else
                SetControlState(Control::State::Hover);
        }
        else
        {
            m_clicked = false;
            m_deltaClickDuration = 0;

            SetControlState(Control::State::Normal);
        }

        m_doubleClicked = false;
        InputableContainer::OnMouseButtonReleased(ev);
    }

    void Control::OnControlChildAdded(Control& control)
    {
    }

    void Control::OnControlChildRemove(Control& control)
    {
    }

    void Control::OnControlStateChanged(Control& sender, const Control::State state)
    {
        Invalidate();

        if (const auto parent = dynamic_cast<Control*>(GetParent()))
            parent->OnControlStateChanged(sender, state);
    }

    void Control::OnControlPress(Control& sender, const sf::Event::MouseButtonPressed& ev)
    {
        if (const auto parent = dynamic_cast<Control*>(GetParent()))
            parent->OnControlPress(sender, ev);
    }

    void Control::OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev)
    {
        if (const auto parent = dynamic_cast<Control*>(GetParent()))
            parent->OnControlClick(sender, ev);
    }

    void Control::OnControlDoubleClick(Control& sender, const sf::Event::MouseButtonPressed& ev)
    {
        if (const auto parent = dynamic_cast<Control*>(GetParent()))
            parent->OnControlDoubleClick(sender, ev);
    }

    void Control::OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled& ev)
    {
        if (!IsEnabled())
            return;

        const auto state = GetControlState();
        if (state == Control::State::Hover || state == Control::State::Active)
        {
            auto uiEvent = Event{false, state, ev.delta};
            if (m_onScrollWheel)
                m_onScrollWheel(*this, uiEvent);

            if (uiEvent.Handled)
                return;
        }

        Inputable::OnMouseWheelScrolled(ev);
    }
}
