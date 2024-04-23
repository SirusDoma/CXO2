#ifndef O2JAM_GAUGE_METADATA_HPP
#define O2JAM_GAUGE_METADATA_HPP

#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>

#include <Genode/IO/Json.hpp>
#include <Genode/UI/Gauge.hpp>

struct GaugeMetadata : public SpriteMetadata
{
    float Maximum;
    Gx::Gauge::Orientation Orientation;

    bool Flicker;
    sf::Time AnimationDuration;
    std::vector<Gx::Animation::Frame> AnimationFrames;
};

#endif