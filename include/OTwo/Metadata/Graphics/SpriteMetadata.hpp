#ifndef O2JAM_SPRITE_METADATA_HPP
#define O2JAM_SPRITE_METADATA_HPP

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

#include <Genode/Graphics/BlendMode.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>


struct SpriteMetadata : TransformMetadata
{
    ~SpriteMetadata() override = default;

    sf::Color     Color;
    sf::IntRect   TexCoords;
    Gx::BlendMode BlendMode;
};

#endif