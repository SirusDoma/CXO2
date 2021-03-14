#include <Genode/UI/Control.hpp>

namespace Gx
{
    Control::Control() :
        m_enabled(true),
        m_visible(true)
    {
    }

    Control::~Control()
    {
    }

    void Control::AddChild(Node *child)
    {
        auto ui = dynamic_cast<Control*>(child);
        if (ui)
        {
            ui->SetEnabled(IsEnabled());
            ui->SetVisible(IsVislble());

            Node::AddChild(child);
        }
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
}
