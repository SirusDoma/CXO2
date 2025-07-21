#pragma once

#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>

#include <Genode/IO/Json.hpp>
#include <Genode/UI/Gauge.hpp>

struct GaugeMetadata : public SpriteMetadata
{
    float Maximum                      = 100.f;
    Gx::Gauge::Orientation Orientation = Gx::Gauge::Orientation::Vertical;
    bool Flicker                       = false;
    sf::Time AnimationDuration         = sf::Time::Zero;
    std::vector<Gx::Animation::Frame> AnimationFrames = {};
};
