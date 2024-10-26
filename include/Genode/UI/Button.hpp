#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/Graphics/Sprite.hpp>
#include <Genode/UI/Control.hpp>

#include <functional>
#include <unordered_map>

namespace Gx
{
    class Button : public virtual Control, public virtual Colorable
    {
    public:
        struct Frame
        {
            sf::IntRect  TexCoords = sf::IntRect();
            sf::Color    Color     = sf::Color::White;
            sf::Vector2f Bounds    = sf::Vector2f();
        };

        Button() = default;
        explicit Button(const sf::Texture& texture);

        const sf::Color& GetColor() const override;
        void SetColor(const sf::Color& color) override;

        const sf::Texture* GetTexture() const;
        const sf::IntRect& GetTextCoords() const;
        sf::FloatRect GetLocalBounds() const override;

        void SetTexture(const sf::Texture& texture);
        void SetFrame(Control::State state, const Frame& frame);

        void PerformClick();

    protected:
        virtual Button::Frame GetCurrentFrame() const;
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

        Frame GetStateFrame(Control::State state) const;
        void Invalidate() override;

     private:
        void UpdatePositions();
        void UpdateTexCoords();

        std::array<sf::Vertex, 4> m_vertices;
        const sf::Texture* m_texture;
        mutable std::unordered_map<Control::State, Frame> m_frames;
    };
}
