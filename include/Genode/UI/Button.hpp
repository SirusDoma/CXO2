#ifndef GENODE_BUTTON_HPP
#define GENODE_BUTTON_HPP

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/Graphics/Sprite.hpp>
#include <Genode/UI/Control.hpp>

#include <functional>
#include <unordered_map>

namespace Gx
{
    class Button : public Control
    {
    public:
        Button();
        explicit Button(const sf::Texture& texture);
        Button(const sf::Texture& texture, const sf::IntRect& rectangle);

        explicit Button(TextureHandle texture);
        Button(TextureHandle texture, const sf::IntRect& rectangle);

        const sf::FloatRect GetLocalBounds() const;

        void SetStateFrame(Button::State state, sf::IntRect texCoords, sf::Color color = sf::Color::White);
        void SetStateFrame(Button::State state, const Gx::Sprite& sprite);

    protected:
        const Sprite GetStateFrame(Button::State state) const;
        Sprite *GetSprite() const;

        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual void Invalidate();

    private:
        mutable Gx::Sprite m_sprite;
        mutable std::unordered_map<Button::State, Sprite> m_stateData;
    };
}

#endif
