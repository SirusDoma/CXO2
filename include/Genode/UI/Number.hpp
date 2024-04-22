#ifndef GENODE_UI_NUMBER_HPP
#define GENODE_UI_NUMBER_HPP

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <Genode/Graphics/Sprite.hpp>
#include <Genode/UI/Control.hpp>

#include <unordered_map>

namespace Gx
{
    class Number : public virtual Control, public Colorable
    {
    public:
        enum class Alignment { None, Left, Center, Right };

        Number();
        explicit Number(const sf::Texture& texture);

        sf::FloatRect GetLocalBounds() const override;

        const sf::Texture* GetTexture() const;
        void SetTexture(const sf::Texture& texture);

        const sf::Color & GetColor() const override;
        void SetColor(const sf::Color &color) override;

        float GetKerning() const;
        void SetKerning(const float value);

        int GetDigitCount() const;
        void SetDigitCount(int count);

        void SetDigitFrame(unsigned int digit, sf::IntRect texCoords);
        void SetDigitsSize(sf::Vector2u size);

        unsigned int GetValue() const;
        void SetValue(unsigned int value);

        void SetAlignment(const Alignment alignment);
        Alignment GetAlignment() const;

    protected:
        void Update(double delta) override;
        RenderStates Render(sf::RenderTarget &target, RenderStates states) const override;
        void Invalidate() override;

    private:
        sf::VertexArray   m_vertices;
        const sf::Texture *m_texture;
        Alignment         m_alignment;

        unsigned int m_value, m_digitCount;
        float m_width, m_height, m_kerning;
        bool m_needUpdate;

        std::unordered_map<unsigned int, sf::IntRect> m_texCoords;
    };
}

#endif
