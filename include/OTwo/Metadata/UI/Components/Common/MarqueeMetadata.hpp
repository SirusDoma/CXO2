#pragma once

#include <OTwo/Metadata/UI/LabelMetadata.hpp>

#include <SFML/Graphics/Rect.hpp>

struct MarqueeMetadata : public LabelMetadata
{
    double        Speed;
    sf::FloatRect Bounds;
};
