#ifndef GENODE_UI_PROGRESS_BAR_HPP
#define GENODE_UI_PROGRESS_BAR_HPP

#include <SFML/Graphics.hpp>

#include <Genode/Graphics/Sprite.hpp>
#include <Genode/UI/Control.hpp>

namespace Gx
{
    class ProgressBar : public virtual Control, public virtual Colorable
    {
    public:
        enum Orientation { Horizontal, Vertical };

        ProgressBar();
        explicit ProgressBar(const sf::Texture& texture);
        ProgressBar(const sf::Texture& texture, const sf::IntRect &texCoords);

        virtual const sf::FloatRect GetLocalBounds() const;

        const sf::Texture* GetTexture() const;
        void SetTexture(const sf::Texture& texture);

        const sf::IntRect& GetTexCoords() const;
        void SetTexCoords(const sf::IntRect &texCoords);

        const sf::Color& GetColor() const;
        void SetColor(const sf::Color &color);

        const Orientation GetOrientation() const;
        void SetOrientation(const Orientation &orientation);

        float GetMaximumValue() const;
        void SetMaximumValue(float max);

        float GetValue() const;
        void SetValue(float value);

    protected:
        virtual void Update(double delta);
        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

        virtual void Invalidate();
    private:
        sf::VertexArray    m_vertices;
        const sf::Texture* m_texture;
        sf::IntRect        m_texCoords;
        Orientation        m_orientation;

    private:
        float m_value, m_maximum;
    };
}

#endif
