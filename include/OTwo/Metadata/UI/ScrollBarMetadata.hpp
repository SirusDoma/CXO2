#pragma once

#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>
#include <Genode/UI/ScrollBar.hpp>
#include <SFML/Graphics/Rect.hpp>

struct ScrollBarMetadata : public SpriteMetadata
{
    float Step         = 1.f;
    float Maximum      = 100.f;
    sf::IntRect Bounds = sf::IntRect();
    Gx::ScrollBar::ScrollOrientation Orientation = Gx::ScrollBar::ScrollOrientation::Vertical;
};
