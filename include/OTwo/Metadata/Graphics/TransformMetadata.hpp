#pragma once

#include <OTwo/Metadata/ResourceMetadata.hpp>

struct TransformMetadata : public ResourceMetadata
{
    virtual ~TransformMetadata() = default;

    sf::Vector2f Position;
    sf::Vector2f Scale     = sf::Vector2f(1.f, 1.f);
    float        Rotation  = 0;
    sf::Vector2f Origin;
};
