#ifndef O2JAM_ANIMATION_METADATA_HPP
#define O2JAM_ANIMATION_METADATA_HPP

#include <Genode/Graphics/Animation.hpp>
#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>
#include <SFML/System/Time.hpp>
#include <vector>

struct AnimationMetadata : SpriteMetadata
{
    ~AnimationMetadata() override = default;

    bool     IsLoop;
    sf::Time Duration;
    float    Speed;
    std::vector<Gx::Animation::Frame> Frames;
};

#endif