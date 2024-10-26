#pragma once

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

#include <Genode/Graphics/BlendMode.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

struct SpriteMetadata : TransformMetadata
{
    sf::Color     Color;
    sf::IntRect   TexCoords;
    Gx::BlendMode BlendMode;
};
