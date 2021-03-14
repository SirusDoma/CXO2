#include <Genode/UI/UiElement.hpp>

namespace Gx
{
    UiElement::UiElement() :
        m_enabled(true),
        m_visible(true)
    {
    }

    UiElement::~UiElement()
    {
    }

    void UiElement::AddChild(Node *child)
    {
        auto ui = dynamic_cast<UiElement*>(child);
        if (ui)
        {
            ui->SetEnabled(IsEnabled());
            ui->SetVisible(IsVislble());

            Node::AddChild(child);
        }
    }

    void UiElement::SetEnabled(bool enabled)
    {
        m_enabled = enabled;
    }

    const bool &UiElement::IsEnabled() const
    {
        return m_enabled;
    }

    void UiElement::SetVisible(bool visible)
    {
        m_visible = visible;
    }

    const bool &UiElement::IsVislble() const
    {
        return m_visible;
    }
}
