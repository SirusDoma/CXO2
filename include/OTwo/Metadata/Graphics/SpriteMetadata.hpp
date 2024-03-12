#ifndef O2JAM_SPRITE_METADATA_HPP
#define O2JAM_SPRITE_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

struct SpriteMetadata : TransformMetadata
{
    ~SpriteMetadata() override = default;

    sf::Color   Color;
    sf::IntRect TexCoords;
};

#endif