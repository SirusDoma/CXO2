#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/System/Application.hpp>

#include <functional>

namespace Gx
{
    Dialog::Dialog() :
        m_overlayMode(true),
        m_accepted(false),
        m_sprite(),
        m_scene(),
        m_acceptButton(),
        m_cancelButton()
    {
    }

    Dialog::Dialog(const sf::Texture &texture) :
        m_overlayMode(true),
        m_accepted(false),
        m_sprite(texture),
        m_scene(),
        m_acceptButton(),
        m_cancelButton()
    {
    }

    Dialog::Dialog(const sf::Texture &texture, const sf::IntRect &rectangle) :
        m_overlayMode(true),
        m_accepted(false),
        m_sprite(texture, rectangle),
        m_scene(),
        m_acceptButton(),
        m_cancelButton()
    {
    }

    Dialog::Dialog(TextureHandle texture) :
        m_overlayMode(true),
        m_accepted(false),
        m_sprite(texture),
        m_scene(),
        m_acceptButton(),
        m_cancelButton()
    {
    }

    Dialog::Dialog(TextureHandle texture, const sf::IntRect &rectangle) :
        m_overlayMode(true),
        m_accepted(false),
        m_sprite(texture, rectangle),
        m_scene(),
        m_acceptButton(),
        m_cancelButton()
    {
    }

    Dialog::~Dialog()
    {
    }

    bool Dialog::IsAccepted() const
    {
        return m_accepted;
    }

    void Dialog::SetOverlayMode(bool enabled)
    {
        m_overlayMode = enabled;
    }

    bool Dialog::IsOverlayMode() const
    {
        return m_overlayMode;
    }

    const sf::FloatRect Dialog::GetLocalBounds() const
    {
        return m_sprite.GetLocalBounds();
    }

    void Dialog::SetAcceptButton(Button *acceptButton)
    {
        if (!acceptButton)
            return;

        if (m_acceptButton)
            RemoveChild(m_acceptButton);

        m_acceptButton = acceptButton;
        m_acceptButton->SetClickCallback([this]() { OnAccepted(); });
        AddChild(m_acceptButton);
    }

    void Dialog::SetCancelButton(Button *cancelButton)
    {
        if (!cancelButton)
            return;

        if (m_cancelButton)
            RemoveChild(m_cancelButton);

        m_cancelButton = cancelButton;
        m_cancelButton->SetClickCallback([this]() { OnCancelled(); });
        AddChild(m_cancelButton);
    }

    void Dialog::Show(Scene *scene, bool enableBackDrop)
    {
        // TODO: Make this normal child with AddChild instead
        m_scene = scene;
        if (m_scene)
        {
            float x = (m_scene->GetView().getSize().x / 2.f) - (GetLocalBounds().width / 2.f);
            float y = (m_scene->GetView().getSize().y / 2.f) - (GetLocalBounds().height / 2.f);

            SetPosition(x, y);
            m_scene->SetOverlay(this);
        }
    }

    void Dialog::Close()
    {
        m_scene->CloseOverlay();
    }

    sf::RenderStates Dialog::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (!IsVislble())
            return states;

        states.transform *= GetTransform();
        target.draw(m_sprite, states);

        states = RenderableContainer::Render(target, states);
        return states;
    }

    void Dialog::OnAccepted()
    {
        m_accepted = true;
        Close();
    }

    void Dialog::OnCancelled()
    {
        m_accepted = false;
        Close();
    }

    void Dialog::Invalidate()
    {
        m_backdrop.SetFillColor(GetBackdropColor());
    }
}