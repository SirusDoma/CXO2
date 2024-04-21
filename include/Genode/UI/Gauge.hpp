#ifndef GENODE_UI_GAUGE_HPP
#define GENODE_UI_GAUGE_HPP

#include <SFML/Graphics.hpp>

#include <Genode/Graphics/Sprite.hpp>
#include <Genode/UI/Control.hpp>

namespace Gx
{
    class Gauge : public virtual Control, public virtual Colorable
    {
    public:
        enum class Orientation { Horizontal, Vertical };

        Gauge();
        explicit Gauge(const sf::Texture& texture);
        Gauge(const sf::Texture& texture, const sf::IntRect &texCoords);

        sf::FloatRect GetLocalBounds() const override;

        const sf::Texture* GetTexture() const;
        void SetTexture(const sf::Texture& texture);

        const sf::IntRect& GetTexCoords() const;
        void SetTexCoords(const sf::IntRect &texCoords);

        const sf::Color& GetColor() const override;
        void SetColor(const sf::Color &color) override;

        const Orientation GetOrientation() const;
        void SetOrientation(const Orientation &orientation);

        float GetMaximumValue() const;
        void SetMaximumValue(float max);

        float GetValue() const;
        void SetValue(float value);

    protected:
        void Update(const double delta) override;
        RenderStates Render(sf::RenderTarget &target, RenderStates states) const override;

        void Invalidate() override;
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
