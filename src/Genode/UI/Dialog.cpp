#include <Genode/UI/Dialog.hpp>

#include <Genode/UI/Label.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/System/Application.hpp>

#include <functional>

namespace Gx
{
    Dialog::Dialog(Dialog&& other) noexcept :
        Node(std::move(other)),
        UiContainer(std::move(other)),
        Sprite(std::move(other)),
        m_acceptButton(std::exchange(other.m_acceptButton, nullptr)),
        m_cancelButton(std::exchange(other.m_cancelButton, nullptr)),
        m_promptLabel(std::exchange(other.m_promptLabel, nullptr)),
        m_parent(std::exchange(other.m_parent, nullptr)),
        m_accepted(std::exchange(other.m_accepted, false)),
        m_shown(std::exchange(other.m_shown, false))
    {
        // Rewire callbacks due to move constructor
        if (m_acceptButton)
            SetAcceptButton(*m_acceptButton);

        if (m_cancelButton)
            SetCancelButton(*m_cancelButton);
    }

    Dialog::Dialog(const sf::Texture& texture) :
        Sprite(texture)
    {
    }

    Dialog::Dialog(const sf::Texture& texture, const sf::IntRect& rectangle) :
        Sprite(texture, rectangle)
    {
    }

    Dialog& Dialog::operator=(Dialog&& other) noexcept
    {
        if (this != &other)
        {
            Node::operator=(std::move(other));
            UiContainer::operator=(std::move(other));
            Sprite::operator=(std::move(other));

            m_acceptButton = std::exchange(other.m_acceptButton, nullptr);
            m_cancelButton = std::exchange(other.m_cancelButton, nullptr);
            m_promptLabel  = std::exchange(other.m_promptLabel, nullptr);
            m_parent       = std::exchange(other.m_parent, nullptr);
            m_accepted     = std::exchange(other.m_accepted, false);
            m_shown        = std::exchange(other.m_shown, false);

            // Rewire callbacks due to move constructor
            if (m_acceptButton)
                SetAcceptButton(*m_acceptButton);

            if (m_cancelButton)
                SetCancelButton(*m_cancelButton);
        }

        return *this;
    }

    Presentable::Parent* Dialog::GetPresentableParent() const
    {
        return m_parent;
    }

    bool Dialog::IsShown() const
    {
        return m_shown;
    }

    bool Dialog::IsAccepted() const
    {
        return m_accepted;
    }

    void Dialog::SetLabel(Label& label)
    {
        if (m_promptLabel)
            RemoveChild(*m_promptLabel);

        m_promptLabel = &label;
        AddChild(label);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void Dialog::SetPromptString(const std::string& prompt)
    {
        if (!m_promptLabel)
            return;

        m_promptLabel->SetString(prompt);
    }

    void Dialog::SetAcceptButton(Button& acceptButton)
    {
        if (m_acceptButton)
            RemoveChild(*m_acceptButton);

        m_acceptButton = &acceptButton;
        m_acceptButton->SetClickCallback([&] (auto& sender, auto& ev) { OnAccepted(); });

        AddChild(acceptButton);
    }

    void Dialog::SetCancelButton(Button& cancelButton)
    {
        if (m_cancelButton)
            RemoveChild(*m_cancelButton);

        m_cancelButton = &cancelButton;
        m_cancelButton->SetClickCallback([&] (auto& sender, auto& ev) { OnCancelled(); });

        AddChild(cancelButton);
    }

    void Dialog::SetAcceptCallback(std::function<void()> callback)
    {
        m_onAccepted = std::move(callback);
    }

    void Dialog::SetCancelCallback(std::function<void()> callback)
    {
        m_onCancelled = std::move(callback);
    }

    void Dialog::OnPresented(Parent& parent, const PresentationContext& context)
    {
        if (m_shown)
            return;

        if (const auto ctx = dynamic_cast<const GraphicalPresentationContext*>(&context))
        {
            const auto center    = ctx->Bounds.size / 2.f;
            const unsigned int x = static_cast<unsigned int>(center.x - (GetLocalBounds().size.x / 2.f));
            const unsigned int y = static_cast<unsigned int>(center.y - (GetLocalBounds().size.y / 2.f));

            SetOrigin(0.f, 0.f);
            SetPosition(x, y);

            if (const auto dctx = dynamic_cast<const DialogPresentationContext*>(ctx))
            {
                if (dctx->UseBackdrop)
                {
                    m_backdrop = Rectangle(ctx->Bounds.size);
                    m_backdrop.SetColor(sf::Color(0, 0, 0, 255 / 2));
                }
                else
                    m_backdrop = Rectangle(sf::Vector2f(0, 0));

                if (!dctx->Prompt.empty() && m_promptLabel)
                    m_promptLabel->SetString(dctx->Prompt);
            }
        }

        m_parent = &parent;
        m_shown  = true;
        Invalidate();
    }

    void Dialog::OnDismissed(Parent& parent)
    {
        m_shown = false;
    }

    bool Dialog::Dismiss()
    {
        if (!m_parent || !m_parent->IsPresenting(*this))
            return false;

        const bool result = m_parent->Dismiss();

        m_shown  = false;
        m_parent = nullptr;

        return result;
    }

    RenderStates Dialog::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        if (m_backdrop.GetSize().x > 0 && m_backdrop.GetSize().y > 0)
            surface.Render(m_backdrop, sf::Transform::Identity);

        return Sprite::Render(surface, states);
    }

    void Dialog::OnKeyPressed(const sf::Event::KeyPressed& ev)
    {
        UiContainer::OnKeyPressed(ev);

        if (ev.code == sf::Keyboard::Key::Enter)
            OnAccepted();
        else if (ev.code == sf::Keyboard::Key::Escape)
            OnCancelled();
    }

    void Dialog::OnAccepted()
    {
        m_accepted = true;
        if (m_onAccepted)
            m_onAccepted();

        Dismiss();
    }

    void Dialog::OnCancelled()
    {
        m_accepted = false;
        if (m_onCancelled)
            m_onCancelled();

        Dismiss();
    }

    std::string Dialog::GetPromptString() const
    {
        if (m_promptLabel)
            return m_promptLabel->GetString();

        return {};
    }

    Label* Dialog::GetLabel() const
    {
        return m_promptLabel;
    }

    Button* Dialog::GetAcceptButton() const
    {
        return m_acceptButton;
    }

    Button* Dialog::GetCancelButton() const
    {
        return m_cancelButton;
    }

    void Dialog::Invalidate()
    {
        UiContainer::Invalidate();
    }
}
