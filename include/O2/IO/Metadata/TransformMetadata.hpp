#ifndef TRANSFORM_METADATA_HPP
#define TRANSFORM_METADATA_HPP

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

struct TransformMetadata : public Gx::ResourceMetadata
{
public:
    TransformMetadata() {}
    virtual ~TransformMetadata() {}

    sf::Vector2f Position;
    sf::Vector2f Scale;
    float        Rotation;
    sf::Vector2f Origin;
};

#endif