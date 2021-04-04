#ifndef O2JAM_SPRITE_METADATA_HPP
#define O2JAM_SPRITE_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <O2/Metadata/TransformMetadata.hpp>

class SpriteMetadata : public TransformMetadata
{
public:
    SpriteMetadata() {}
    virtual ~SpriteMetadata() {}

    const sf::Color& GetColor() const      { return m_color; }
    void SetColor(const sf::Color& mColor) { m_color = mColor; }

    const sf::IntRect& GetTexCoords() const          { return m_texCoords; }
    void SetTexCoords(const sf::IntRect& mTexCoords) { m_texCoords = mTexCoords; }

private:
    sf::Color   m_color;
    sf::IntRect m_texCoords;
};

#endif