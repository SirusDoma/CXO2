#ifndef GENODE_UI_DIALOG_HPP
#define GENODE_UI_DIALOG_HPP

#include <Genode/UI/UiContainer.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <functional>

namespace Gx
{
    class Scene;
    class Button;
    class Dialog : public virtual UiContainer
    {
    public:
        Dialog();
        explicit Dialog(const Dialog &copy);
        explicit Dialog(const sf::Texture& texture);
        Dialog(const sf::Texture& texture, const sf::IntRect& rectangle);

        ~Dialog() override = default;

        const sf::Texture* GetTexture() const;
        void SetTexture(const sf::Texture& texture);

        const sf::IntRect& GetTexCoords() const;
        void SetTexCoords(const sf::IntRect& rectangle);

        bool IsAccepted() const;
        sf::FloatRect GetLocalBounds() const override;

        Label *GetLabel() const;
        Button *GetAcceptButton() const;
        Button *GetCancelButton() const;

        void SetLabel(Label &label);
        void SetPromptString(const std::string& prompt);
        void SetAcceptButton(Button &acceptButton);
        void SetCancelButton(Button &cancelButton);

        void SetAcceptCallback(std::function<void()> callback);
        void SetCancelCallback(std::function<void()> callback);

        void Show(Scene *scene);
        void Show(Scene *scene, const std::string& prompt = std::string(), bool enableBackdrop = true);
        bool IsShown() const;

        void Close();

    protected:
        void OnKeyDown(sf::Event::KeyEvent ev) override;
        RenderStates Render(RenderSurface &surface, RenderStates states) const override;

        virtual void OnShown(Scene &scene);
        virtual void OnClose();

        virtual void OnAccepted();
        virtual void OnCancelled();

        const Scene *GetScene();

        void Invalidate() override;

    private:
        mutable Gx::Sprite m_sprite;

        Button *m_acceptButton, *m_cancelButton;
        Label  *m_promptText;

        Scene *m_scene;
        Rectangle m_backdrop;

        bool m_accepted, m_shown;
        std::function<void()> m_onAccepted, m_onCancelled;
    };
}

#endif
