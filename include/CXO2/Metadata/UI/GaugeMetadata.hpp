#pragma once

#include <CXO2/Metadata/Graphics/SpriteMetadata.hpp>

#include <Genode/IO/Json.hpp>
#include <CXO2/UI/Gauge.hpp>

namespace Cx
{
    struct GaugeMetadata : public SpriteMetadata
    {
        float Maximum                      = 100.f;
        Gauge::Orientation Orientation     = Gauge::Orientation::Vertical;
        bool Flicker                       = false;
        sf::Time AnimationDuration         = sf::Time::Zero;
        std::vector<Gx::Animation::Frame> AnimationFrames = {};
    };
}
