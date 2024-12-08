#pragma once

#include <Genode/Entities/Presentable.hpp>
#include <Genode/UI/UiContainer.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <functional>

namespace Gx
{
    struct DialogPresentationContext : GraphicalPresentationContext
    {
        bool UseBackdrop{false};
        std::string Prompt{};
    };

    class Scene;
    class Label;
    class Button;
    class Dialog : public Presentable, public UiContainer, public virtual Colorable, private Sprite
    {
    public:
        Dialog() = default;
        explicit Dialog(const sf::Texture& texture);
        Dialog(const sf::Texture& texture, const sf::IntRect& rectangle);

        Dialog(const Dialog&) = delete;
        Dialog& operator=(Dialog&) noexcept;

        Dialog(Dialog&& other) noexcept;
        Dialog& operator=(Dialog&& other) noexcept;

        Parent* GetPresentableParent() const;

        using Control::GetGlobalBounds;
        using Sprite::GetLocalBounds;

        using Sprite::GetTexture;
        using Sprite::SetTexture;

        using Sprite::GetTexCoords;
        using Sprite::SetTexCoords;

        using Sprite::GetColor;
        using Sprite::SetColor;

        bool IsShown() const;
        bool IsAccepted() const;

        Label* GetLabel() const;
        std::string GetPromptString() const;
        Button* GetAcceptButton() const;
        Button* GetCancelButton() const;

        void SetLabel(Label& label);
        void SetPromptString(const std::string& prompt);
        void SetAcceptButton(Button& acceptButton);
        void SetCancelButton(Button& cancelButton);

        void SetAcceptCallback(std::function<void()> callback);
        void SetCancelCallback(std::function<void()> callback);

        bool Dismiss() override;

    protected:
        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

        void OnPresented(Parent& parent, const PresentationContext& context) override;
        void OnDismissed(Parent& parent) override;

        virtual void OnAccepted();
        virtual void OnCancelled();

        void Invalidate() override;

    private:
        Button* m_acceptButton{};
        Button* m_cancelButton{};
        Label* m_promptLabel{};

        Presentable::Parent* m_parent{};
        Rectangle m_backdrop{};

        bool m_accepted{};
        bool m_shown{};
        std::function<void()> m_onAccepted{};
        std::function<void()> m_onCancelled{};
    };
}
