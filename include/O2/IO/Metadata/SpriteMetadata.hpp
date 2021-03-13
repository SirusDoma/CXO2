#ifndef SPRITE_METADATA_HPP
#define SPRITE_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <O2/IO/Metadata/TransformMetadata.hpp>

class SpriteMetadata : public TransformMetadata
{
public:
    SpriteMetadata() {}
    virtual ~SpriteMetadata() {}

    sf::Color   Color;
    sf::IntRect TexCoords;
};

#endif