#ifndef O2JAM_SPRITE_METADATA_HPP
#define O2JAM_SPRITE_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <O2/Metadata/TransformMetadata.hpp>

struct SpriteMetadata : public TransformMetadata
{
    SpriteMetadata() {}
    virtual ~SpriteMetadata() {}

    sf::Color   Color;
    sf::IntRect TexCoords;
};

#endif