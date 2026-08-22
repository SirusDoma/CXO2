#pragma once

#include <Genode/Entities/Presentable.hpp>
#include <CXO2/UI/UiContainer.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <SFML/System/String.hpp>

#include <functional>

namespace Cx
{
    struct DialogPresentationContext : Gx::GraphicalPresentationContext
    {
        bool UseBackdrop{false};
        sf::String Prompt{};
    };

    class Label;
    class Button;
    class Dialog : public Gx::Presentable, public UiContainer, public virtual Gx::Colorable, private Gx::Sprite
    {
    public:
        Dialog() = default;
        explicit Dialog(const sf::Texture& texture);
        Dialog(const sf::Texture& texture, const sf::IntRect& rectangle);

        Dialog(const Dialog&) = delete;
        Dialog& operator=(const Dialog&) = delete;

        Dialog(Dialog&& other) noexcept;
        Dialog& operator=(Dialog&& other) noexcept;

        [[nodiscard]] Parent* GetPresentableParent() const;

        using Control::GetGlobalBounds;
        using Gx::Sprite::GetLocalBounds;

        using Gx::Sprite::GetTexture;
        using Gx::Sprite::SetTexture;

        using Gx::Sprite::GetTexCoords;
        using Gx::Sprite::SetTexCoords;

        using Gx::Sprite::GetColor;
        using Gx::Sprite::SetColor;

        using Gx::Sprite::GetBlendMode;
        using Gx::Sprite::SetBlendMode;

        [[nodiscard]] bool IsShown() const;
        [[nodiscard]] bool IsAccepted() const;

        [[nodiscard]] Label* GetLabel() const;
        [[nodiscard]] sf::String GetPromptString() const;
        [[nodiscard]] Button* GetAcceptButton() const;
        [[nodiscard]] Button* GetCancelButton() const;

        void SetLabel(Label& label);
        void SetPromptString(const sf::String& prompt);
        void SetAcceptButton(Button& acceptButton);
        void SetCancelButton(Button& cancelButton);

        void SetAcceptCallback(std::function<void(Control&, Control::Event&)> callback);
        void SetCancelCallback(std::function<void(Control&, Control::Event&)> callback);

        bool Dismiss() override;

    protected:
        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        void OnPresented(Parent& parent, const Gx::PresentationContext& context) override;
        void OnDismissed(Parent& parent) override;

        virtual void OnAccepted();
        virtual void OnCancelled();

        void Invalidate() override;

    private:
        Button* m_acceptButton{};
        Button* m_cancelButton{};
        Label* m_promptLabel{};

        Gx::Presentable::Parent* m_parent{};
        Gx::Rectangle m_backdrop{};

        bool m_accepted{};
        bool m_shown{};
        std::function<void(Control&, Control::Event&)> m_onAccepted{};
        std::function<void(Control&, Control::Event&)> m_onCancelled{};
    };
}
