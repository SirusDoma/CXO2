#ifndef GENODE_BUTTON_HPP
#define GENODE_BUTTON_HPP

#include <Genode/UI/UiElement.hpp>
#include <Genode/Graphics.hpp>

#include <functional>
#include <unordered_map>

namespace Gx
{
    enum ButtonState { NORMAL, HOVER, PRESSED };

    class Button : public UiElement
    {
    public:
        Button();
        explicit Button(const sf::Texture& texture);
        Button(const sf::Texture& texture, const sf::IntRect& rectangle);

        explicit Button(TextureHandle texture);
        Button(TextureHandle texture, const sf::IntRect& rectangle);

        void SetFocusCallback(std::function<void(Button*)> callback);
        void SetLostFocusCallback(std::function<void(Button*)> callback);
        void SetClickCallback(std::function<void(Button*)> callback);

        void AddButtonState(ButtonState state, sf::IntRect texCoords, sf::Color color = sf::Color::White);
        void AddButtonState(ButtonState state, const Gx::Sprite& sprite);

    protected:
        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual void Update(double delta);

    private:
        mutable Gx::Sprite m_sprite;
        mutable std::unordered_map<ButtonState, Gx::Sprite> m_stateData;

        ButtonState m_state;
        std::function<void(Button*)> m_focus, m_lostFocus, m_click;

        virtual void OnMouseMove(sf::Event::MouseMoveEvent ev);
        virtual void OnMouseButtonClick(sf::Event::MouseButtonEvent ev);
        virtual void OnMouseButtonUp(sf::Event::MouseButtonEvent ev);

        bool IsIntersect(sf::Vector2f ev);
    };
}

#endif
