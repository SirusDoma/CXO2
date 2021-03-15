#include <Genode/UI/Control.hpp>

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

    void Control::AddChild(Node *child)
    {
        auto ui = dynamic_cast<Control*>(child);
        if (!ui)
            return;

        ui->SetEnabled(IsEnabled());
        ui->SetVisible(IsVislble());

        Node::AddChild(child);
    }

    void Control::SetEnabled(bool enabled)
    {
        m_enabled = enabled;
    }

    const bool &Control::IsEnabled() const
    {
        return m_enabled;
    }

    void Control::SetVisible(bool visible)
    {
        m_visible = visible;
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

    void Control::OnMouseMove(sf::Event::MouseMoveEvent ev)
    {
        Inputable::OnMouseMove(ev);
        if (!IsEnabled() || GetControlState() == Control::State::Active)
            return;

        bool intersect = GetGlobalBounds().contains(ev.x, ev.y);
        if (intersect && GetControlState() == Control::State::Normal)
            SetControlState(Control::State::Hover);
        else if (!intersect && GetControlState() == Control::State::Hover)
            SetControlState(Control::State::Normal);
    }

    void Control::OnMouseButtonClick(sf::Event::MouseButtonEvent ev)
    {
        Inputable::OnMouseButtonClick(ev);
        if (IsEnabled() && m_state == Control::State::Hover && GetGlobalBounds().contains(ev.x, ev.y))
        {
            SetControlState(Control::State::Active);
            OnControlPress(ev);
        }
    }

    void Control::OnMouseButtonUp(sf::Event::MouseButtonEvent ev)
    {
        Inputable::OnMouseButtonUp(ev);
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
    }

    void Control::Update(double delta)
    {
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
