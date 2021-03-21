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
        Number();
        explicit Number(const sf::Texture& texture);
        explicit Number(TextureHandle texture);

        virtual const sf::FloatRect GetLocalBounds() const;

        const sf::Texture* GetTexture() const;
        void SetTexture(const sf::Texture& texture);
        void SetTexture(TextureHandle texture);

        virtual const sf::Color & GetColor() const;
        virtual void SetColor(const sf::Color &color);

        float GetLetterSpacing() const;
        void SetLetterSpacing(float spacing);

        int GetDigitCount() const;
        void SetDigitCount(int count);

        void SetDigitFrame(unsigned int digit, sf::IntRect texCoords);
        void SetDigitsSize(sf::Vector2u size);

        unsigned int GetValue() const;
        void SetValue(unsigned int value);

    protected:
        virtual void Update(double delta);
        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;
        virtual void Invalidate();

    private:
        sf::VertexArray m_vertices;
        TextureHandle   m_texture;

        unsigned int m_value, m_digitCount;
        float m_width, m_height, m_spacing;
        bool m_needUpdate;

        std::unordered_map<unsigned int, sf::IntRect> m_texCoords;
    };
}

#endif
