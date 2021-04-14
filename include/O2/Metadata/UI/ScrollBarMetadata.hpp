#ifndef O2JAM_SCROLL_BAR_METADATA_HPP
#define O2JAM_SCROLL_BAR_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/UI/ScrollBar.hpp>
#include <O2/Metadata/SpriteMetadata.hpp>

struct ScrollBarMetadata : public SpriteMetadata
{
    float Step, Maximum;
    sf::FloatRect Bounds;
    Gx::ScrollBar::ScrollOrientation Orientation;
};

#endif