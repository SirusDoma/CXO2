#pragma once

#include <CXO2/Metadata/UI/LabelMetadata.hpp>

#include <SFML/Graphics/Rect.hpp>

namespace Cx
{
    struct MarqueeMetadata : public LabelMetadata
    {
        double      Speed = 30.0f;
        sf::IntRect Bounds = sf::IntRect();
    };
}
