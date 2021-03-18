#ifndef GENODE_UI_DIALOG_HPP
#define GENODE_UI_DIALOG_HPP

#include <Genode/UI/UiContainer.hpp>
#include <Genode/Graphics/Sprite.hpp>

namespace Gx
{
    class Scene;
    class Button;
    class Dialog : public UiContainer
    {
    public:
        Dialog();
        explicit Dialog(const sf::Texture& texture);
        Dialog(const sf::Texture& texture, const sf::IntRect& rectangle);

        explicit Dialog(TextureHandle texture);
        Dialog(TextureHandle texture, const sf::IntRect& rectangle);

        virtual ~Dialog();

        bool IsAccepted() const;
        virtual const sf::FloatRect GetLocalBounds() const;

        void SetAcceptButton(Button *acceptButton);
        void SetCancelButton(Button *cancelButton);

        void SetOverlayMode(bool enabled);
        bool IsOverlayMode() const;

        void Show(Scene *scene, bool enableBackdrop = true);
        void Close();

    protected:
        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

        virtual void OnAccepted();
        virtual void OnCancelled();

        virtual void Invalidate();

    private:
        mutable Gx::Sprite m_sprite;

        Scene *m_scene;
        Button *m_acceptButton, *m_cancelButton;
        Rectangle m_backdrop;

        bool m_accepted, m_overlayMode;
    };
}

#endif
