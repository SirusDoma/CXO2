#ifndef O2JAM_ANIMATION_METADATA_HPP
#define O2JAM_ANIMATION_METADATA_HPP

#include <O2/Metadata/SpriteMetadata.hpp>
#include <vector>

struct AnimationMetadata : public SpriteMetadata
{
    AnimationMetadata() : Frames() {}
    virtual ~AnimationMetadata() {}

    bool     IsLoop;
    sf::Time Duration;
    std::vector<Gx::Animation::Frame> Frames;
};

#endif