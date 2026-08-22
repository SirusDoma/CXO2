#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/Graphics/Sprite.hpp>
#include <CXO2/UI/Control.hpp>

#include <functional>
#include <unordered_map>

namespace Cx
{
    class Button : public virtual Control, public virtual Gx::Colorable
    {
    public:
        struct Frame
        {
            sf::IntRect TexCoords = sf::IntRect();
            sf::IntRect Bounds    = sf::IntRect();
        };

        Button() = default;
        explicit Button(const sf::Texture& texture);

        [[nodiscard]] const sf::Color& GetColor() const override;
        void SetColor(const sf::Color& color) override;

        [[nodiscard]] const sf::Texture* GetTexture() const;
        [[nodiscard]] const sf::IntRect& GetTextCoords() const;
        [[nodiscard]] sf::FloatRect GetLocalBounds() const override;

        void SetTexture(const sf::Texture& texture);
        void SetFrame(Control::State state, const Frame& frame);

        void PerformClick();

    protected:
        [[nodiscard]] virtual Button::Frame GetCurrentFrame() const;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        [[nodiscard]] Frame GetStateFrame(Control::State state) const;
        void Invalidate() override;

     private:
        void UpdatePositions();
        void UpdateTexCoords();

        std::array<sf::Vertex, 4> m_vertices;
        const sf::Texture* m_texture;
        mutable std::unordered_map<Control::State, Frame> m_frames;
    };
}
