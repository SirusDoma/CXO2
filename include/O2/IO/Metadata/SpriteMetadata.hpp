#ifndef SPRITE_METADATA_HPP
#define SPRITE_METADATA_HPP

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

class SpriteMetadata : public Gx::ResourceMetadata
{
public:
    SpriteMetadata() {}
    virtual ~SpriteMetadata() {}

    sf::Color   Color;
    sf::IntRect TexCoords;
};

#endif