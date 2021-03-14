#ifndef ANIMATION_METADATA_HPP
#define ANIMATION_METADATA_HPP

#include <SFML/Graphics.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <O2/IO/Metadata/SpriteMetadata.hpp>

#include <vector>

struct AnimationMetadata : public SpriteMetadata
{
public:
    AnimationMetadata() {}
    virtual ~AnimationMetadata() {}

    bool     IsLoop;
    sf::Time Duration;

    std::vector<sf::IntRect> Frames;
};

#endif