#ifndef GENODE_BUTTON_HPP
#define GENODE_BUTTON_HPP

#include <Genode/UI/UiElement.hpp>
#include <Genode/Graphics.hpp>

#include <functional>
#include <unordered_map>

namespace Gx
{
    class Button : public UiElement
    {
    public:
        enum State { Normal, Hover, Pressed };

        Button();
        explicit Button(const sf::Texture& texture);
        Button(const sf::Texture& texture, const sf::IntRect& rectangle);

        explicit Button(TextureHandle texture);
        Button(TextureHandle texture, const sf::IntRect& rectangle);

        void SetClickCallback(std::function<void()> callback);
        void SetStateFrame(Button::State state, sf::IntRect texCoords, sf::Color color = sf::Color::White);
        void SetStateFrame(Button::State state, const Gx::Sprite& sprite);

    protected:
        const Sprite GetStateFrame(Button::State state) const;
        Sprite *GetSprite() const;

        const Button::State GetState() const;
        void SetState(const Button::State &state);

        bool IsIntersect(sf::Vector2f ev);

        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual void Update(double delta);

        virtual void OnMouseMove(sf::Event::MouseMoveEvent ev);
        virtual void OnMouseButtonClick(sf::Event::MouseButtonEvent ev);
        virtual void OnMouseButtonUp(sf::Event::MouseButtonEvent ev);

    private:
        mutable Gx::Sprite m_sprite;
        mutable std::unordered_map<Button::State, Sprite> m_stateData;

        Button::State m_state;
        std::function<void()> m_click;
    };
}

#endif
