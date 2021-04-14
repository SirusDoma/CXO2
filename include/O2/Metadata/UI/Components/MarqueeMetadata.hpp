#ifndef O2JAM_MARQUEE_METADATA_HPP
#define O2JAM_MARQUEE_METADATA_HPP

#include <O2/Metadata/UI/LabelMetadata.hpp>

#include <SFML/Graphics/Rect.hpp>

struct MarqueeMetadata : public LabelMetadata
{
    double        Speed;
    sf::FloatRect Bounds;
};

#endif
