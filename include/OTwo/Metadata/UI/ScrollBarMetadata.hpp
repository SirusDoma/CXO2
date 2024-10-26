#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/UI/ScrollBar.hpp>
#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>

struct ScrollBarMetadata : public SpriteMetadata
{
    float Step, Maximum;
    sf::FloatRect Bounds;
    Gx::ScrollBar::ScrollOrientation Orientation;
};
