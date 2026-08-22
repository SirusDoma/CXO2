#pragma once

#include <CXO2/Metadata/Graphics/SpriteMetadata.hpp>
#include <CXO2/UI/ScrollBar.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace Cx
{
    struct ScrollBarMetadata : public SpriteMetadata
    {
        float Step                               = 1.f;
        float Maximum                            = 100.f;
        sf::IntRect Bounds                       = sf::IntRect();
        ScrollBar::ScrollOrientation Orientation = ScrollBar::ScrollOrientation::Vertical;
    };
}
