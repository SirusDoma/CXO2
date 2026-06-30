#pragma once

#include <CXO2/Metadata/ResourceMetadata.hpp>

namespace Cx
{
    struct TransformMetadata : public ResourceMetadata
    {
        sf::Vector2f Position  = sf::Vector2f(0.f, 0.f);
        sf::Vector2f Scale     = sf::Vector2f(1.f, 1.f);
        float        Rotation  = 0;
        sf::Vector2f Origin    = sf::Vector2f(0.f, 0.f);
    };
}
