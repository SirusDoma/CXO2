#ifndef GENODE_UI_BUTTON_HPP
#define GENODE_UI_BUTTON_HPP

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

        sf::FloatRect GetLocalBounds() const override;

        const sf::Texture* GetTexture() const;
        void SetTexture(const sf::Texture& texture);

        void SetStateFrame(Button::State state, sf::IntRect texCoords, sf::Color color = sf::Color::White);
        void SetStateFrame(Button::State state, const Gx::Sprite& sprite);

        void PerformClick();

    protected:
        const Sprite GetStateFrame(Button::State state) const;
        Sprite *GetSprite() const;

        RenderStates Render(sf::RenderTarget& target, RenderStates states) const override;
        void Invalidate() override;

    private:
        mutable Gx::Sprite m_sprite;
        mutable std::unordered_map<Button::State, Sprite> m_stateData;
    };
}

#endif
