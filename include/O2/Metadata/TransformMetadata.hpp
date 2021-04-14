#ifndef O2JAM_TRANSFORM_METADATA_HPP
#define O2JAM_TRANSFORM_METADATA_HPP

#include <Genode/IO/ResourceMetadata.hpp>

struct TransformMetadata : public Gx::ResourceMetadata
{
    TransformMetadata() {}
    virtual ~TransformMetadata() {}

    sf::Vector2f Position;
    sf::Vector2f Scale;
    float        Rotation;
    sf::Vector2f Origin;
};

#endif