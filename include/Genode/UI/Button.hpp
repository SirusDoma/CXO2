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
    class Button : public virtual Control, public virtual Colorable, Sprite
    {
    public:
        struct Frame
        {
            sf::IntRect TexCoords = sf::IntRect();
            sf::Color   Color     = sf::Color::White;
        };

        Button() = default;
        explicit Button(const sf::Texture& texture);
        Button(const sf::Texture& texture, const sf::IntRect& rectangle);

        ~Button() override = default;

        const sf::Color &GetColor() const override;
        void SetColor(const sf::Color &color) override;

        sf::FloatRect GetLocalBounds() const override;
        // ReSharper disable once CppHidingFunction
        sf::FloatRect GetGlobalBounds() const;

        void SetTexture(const sf::Texture &texture);
        void SetStateFrame(Control::State state, const Frame &frame);

        void PerformClick();

    protected:
        Frame GetStateFrame(Control::State state) const;
        void ApplyFrame(const Button::Frame& frame);

        RenderStates Render(RenderSurface &surface, RenderStates states) const override;
        void Invalidate() override;

    private:
        mutable std::unordered_map<Control::State, Frame> m_stateData;
    };
}

#endif
